#include "types.h"
#include "param.h"
#include "memlayout.h"
#include "riscv.h"
#include "spinlock.h"
#include "proc.h"
#include "defs.h"

struct spinlock tickslock;
uint ticks;

extern char trampoline[], uservec[], userret[];

extern struct proc proc[NPROC];  // ← ADD THIS LINE
extern int sched_mode;

// in kernelvec.S, calls kerneltrap().
void kernelvec();

extern int devintr();

void
trapinit(void)
{
  initlock(&tickslock, "time");
}

// set up to take exceptions and traps while in the kernel.
void
trapinithart(void)
{
  w_stvec((uint64)kernelvec);
}

//
// handle an interrupt, exception, or system call from user space.
// called from trampoline.S
//
void
usertrap(void)
{
  int which_dev = 0;

  if((r_sstatus() & SSTATUS_SPP) != 0)
    panic("usertrap: not from user mode");

  // send interrupts and exceptions to kerneltrap(),
  // since we're now in the kernel.
  w_stvec((uint64)kernelvec);

  struct proc *p = myproc();

  // save user program counter.
  p->trapframe->epc = r_sepc();

  if(r_scause() == 8){
    // system call

    if(killed(p))
      exit(-1);

    // sepc points to the ecall instruction,
    // but we want to return to the next instruction.
    p->trapframe->epc += 4;

    // an interrupt will change sepc, scause, and sstatus,
    // so enable only now that we're done with those registers.
    intr_on();

    syscall();
  } else if((which_dev = devintr()) != 0){
    // ok
  } else {
    printf("usertrap(): unexpected scause 0x%lx pid=%d\n", r_scause(), p->pid);
    printf("            sepc=0x%lx stval=0x%lx\n", r_sepc(), r_stval());
    setkilled(p);
  }

  if(killed(p))
    exit(-1);

  // give up the CPU if this is a timer interrupt.
  if(which_dev == 2)
    yield();

  usertrapret();
}

//
// return to user space
//
void
usertrapret(void)
{
  struct proc *p = myproc();

  // we're about to switch the destination of traps from
  // kerneltrap() to usertrap(), so turn off interrupts until
  // we're back in user space, where usertrap() is correct.
  intr_off();

  // send syscalls, interrupts, and exceptions to uservec in trampoline.S
  uint64 trampoline_uservec = TRAMPOLINE + (uservec - trampoline);
  w_stvec(trampoline_uservec);

  // set up trapframe values that uservec will need when
  // the process next traps into the kernel.
  p->trapframe->kernel_satp = r_satp();         // kernel page table
  p->trapframe->kernel_sp = p->kstack + PGSIZE; // process's kernel stack
  p->trapframe->kernel_trap = (uint64)usertrap;
  p->trapframe->kernel_hartid = r_tp();         // hartid for cpuid()

  // set up the registers that trampoline.S's sret will use
  // to get to user space.

  // set S Previous Privilege mode to User.
  unsigned long x = r_sstatus();
  x &= ~SSTATUS_SPP; // clear SPP to 0 for user mode
  x |= SSTATUS_SPIE; // enable interrupts in user mode
  w_sstatus(x);

  // set S Exception Program Counter to the saved user pc.
  w_sepc(p->trapframe->epc);

  // tell trampoline.S the user page table to switch to.
  uint64 satp = MAKE_SATP(p->pagetable);

  // jump to userret in trampoline.S at the top of memory, which
  // switches to the user page table, restores user registers,
  // and switches to user mode with sret.
  uint64 trampoline_userret = TRAMPOLINE + (userret - trampoline);
  ((void (*)(uint64))trampoline_userret)(satp);
}

// interrupts and exceptions from kernel code go here via kernelvec,
// on whatever the current kernel stack is.
void
kerneltrap()
{
  int which_dev = 0;
  uint64 sepc = r_sepc();
  uint64 sstatus = r_sstatus();
  uint64 scause = r_scause();

  if((sstatus & SSTATUS_SPP) == 0)
    panic("kerneltrap: not from supervisor mode");
  if(intr_get() != 0)
    panic("kerneltrap: interrupts enabled");

  if((which_dev = devintr()) == 0){
    // interrupt or trap from an unknown source
    printf("scause=0x%lx sepc=0x%lx stval=0x%lx\n", scause, r_sepc(), r_stval());
    panic("kerneltrap");
  }

  // give up the CPU if this is a timer interrupt.
  if(which_dev == 2 && myproc() != 0)
    yield();

  // the yield() may have caused some traps to occur,
  // so restore trap registers for use by kernelvec.S's sepc instruction.
  w_sepc(sepc);
  w_sstatus(sstatus);
}

void
clockintr()
{
    // Update global ticks (only on CPU 0)
    if(cpuid() == 0){
        acquire(&tickslock);
        ticks++;

        // DEBUG: Print every 100 ticks
        // if(ticks % 100 == 0) {
        //     printf("TICK %d, sched_mode=%d\n", ticks, sched_mode);
        // }

        update_time();
        wakeup(&ticks);
        release(&tickslock);
    }

    // Handle current running process
    struct proc *p = myproc();
    if(p != 0 && p->state == RUNNING){
        acquire(&p->lock);

        // Update run_time and remaining_time
        p->run_time++;
        p->remaining_time = p->initial_priority - p->run_time;
        if(p->remaining_time < 0)
            p->remaining_time = 0;

        int current_remaining = p->remaining_time;
        int current_pid = p->pid;
        int current_initial = p->initial_priority;
        release(&p->lock);

        // DEBUG: Print process info
        if(current_pid >= 4 && current_pid <= 6 && p->run_time % 50 == 0) {
            printf("  [CLOCK] PID %d: run_time=%d, initial=%d, remaining=%d\n",
                   current_pid, p->run_time, current_initial, current_remaining);
        }

        // Check if preemption is needed (only for priority scheduling)
        if(sched_mode == SCHED_PRIORITY){
            int should_yield = 0;
            int preempt_pid = 0;
            int preempt_remaining = 0;
            struct proc *other;

            // Scan for higher priority (lower remaining time) RUNNABLE process
            for(other = proc; other < &proc[NPROC]; other++){
                if(other == p) continue;  // Skip current process

                acquire(&other->lock);
                if(other->state == RUNNABLE){
                    int other_remaining = other->initial_priority - other->run_time;
                    if(other_remaining < 0) other_remaining = 0;

                    // DEBUG: Show what we found
                    printf("  [CHECK] Found RUNNABLE PID %d: rem=%d vs current PID %d: rem=%d\n",
                           other->pid, other_remaining, current_pid, current_remaining);

                    // If another process has LESS remaining time, preempt
                    if(other_remaining < current_remaining){
                        should_yield = 1;
                        preempt_pid = other->pid;
                        preempt_remaining = other_remaining;
                        release(&other->lock);
                        break;  // Found higher priority process
                    }
                }
                release(&other->lock);
            }

            // Preempt current process if needed
            if(should_yield){
                printf("*** PREEMPT: PID %d (rem=%d) yields to PID %d (rem=%d) ***\n",
                       current_pid, current_remaining, preempt_pid, preempt_remaining);
                yield();
            }
        } else {
            // DEBUG: Why aren't we checking?
            if(current_pid >= 4 && current_pid <= 6) {
                printf("  [SKIP] sched_mode=%d (not PRIORITY)\n", sched_mode);
            }
        }
    }

    // Reset timer for next interrupt
    w_stimecmp(r_time() + 1000000);
}
// check if it's an external interrupt or software interrupt,
// and handle it.
// returns 2 if timer interrupt,
// 1 if other device,
// 0 if not recognized.
int
devintr()
{
  uint64 scause = r_scause();

  if(scause == 0x8000000000000009L){
    // this is a supervisor external interrupt, via PLIC.

    // irq indicates which device interrupted.
    int irq = plic_claim();

    if(irq == UART0_IRQ){
      uartintr();
    } else if(irq == VIRTIO0_IRQ){
      virtio_disk_intr();
    } else if(irq){
      printf("unexpected interrupt irq=%d\n", irq);
    }

    // the PLIC allows each device to raise at most one
    // interrupt at a time; tell the PLIC the device is
    // now allowed to interrupt again.
    if(irq)
      plic_complete(irq);

    return 1;
  } else if(scause == 0x8000000000000005L){
    // timer interrupt.
    clockintr();
    return 2;
  } else {
    return 0;
  }
}
