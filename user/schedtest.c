#include "kernel/types.h"
#include "kernel/stat.h"
#include "user/user.h"
#include "kernel/fcntl.h"


int main(int argc, char *argv[]) {

  int pid;
  int k, nprocess = 10;
  int z, steps = 1000000;
  char buffer_src[1024], buffer_dst[1024];

  int Avg_waiting_time=0;
  int dec_waiting = 0;
  int Avg_turnaround_time=0;
  int decimal_turnaround =0;

  struct sched_stat st;


  for (k = 0; k < nprocess; k++) {
    // ensure different creation times (proc->ctime)
    // needed for properly testing FCFS scheduling
    sleep(2);

    pid = fork();
    if (pid < 0) {
      printf("%d failed in fork!\n", getpid());
      exit(0);

    }
    else if (pid == 0) {
      // child
      printf("[pid=%d] created\n ", getpid());

      for (z = 0; z < steps; z += 1) {
         // copy buffers one inside the other and back
         // used for wasting cpu time
         memmove(buffer_dst, buffer_src, 1024);
         memmove(buffer_src, buffer_dst, 1024);
      }
      exit(0);
    }
  }

  //pass el vars hena

  for (k = 0; k < nprocess; k++) {
    //pid = wait(0);

    pid = getprocinfo(&st);

      // printf("created p %d at %d\n ",st.pid,st.creation_time);


    Avg_waiting_time += st.waiting_time;
    Avg_turnaround_time += st.turnaround_time;


    printf("[pid=%d] terminated, run_time: %d\n", pid, st.run_time);
  }

  Avg_waiting_time = Avg_waiting_time / nprocess;
  dec_waiting = Avg_waiting_time % nprocess;
  printf("Average waiting time  = %d.%d\n", Avg_waiting_time, dec_waiting);

  Avg_turnaround_time = Avg_turnaround_time / nprocess;
  decimal_turnaround = Avg_turnaround_time % nprocess;
  printf("Average turnaround time  = %d.%d\n", Avg_turnaround_time, decimal_turnaround);



  exit(0);


}
