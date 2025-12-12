#include "types.h"
#include "riscv.h"
#include "defs.h"
#include "param.h"
#include "memlayout.h"
#include "spinlock.h"
#include "proc.h"
uint64 sys_getppid(void) {
    struct proc *p = myproc();
    if (p->parent) return p->parent->pid;
    return 0;
}
