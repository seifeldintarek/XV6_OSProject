#include "kernel/types.h"
#include "kernel/stat.h"
#include "user/user.h"

#define NPROC 64

struct proc_info {
  int pid;
  int ppid;
  int state;
  char name[16];
  unsigned long sz;
};

int
main(int argc, char *argv[])
{
  struct proc_info ptable[NPROC];

  if(getptable(NPROC, ptable) == 0){ //Address of ptable array passed to getptable buffer fel proc.c
    printf("getptable failed\n");
    exit(1);
  }

  printf("PID\tPPID\tSTATE\tSIZE\tNAME\n");
  for(int i = 0; i < NPROC; i++){
    if(ptable[i].pid > 0){
      printf("%d\t%d\t%d\t%ld\t%s\n",
             ptable[i].pid,
             ptable[i].ppid,
             ptable[i].state,
             ptable[i].sz,
             ptable[i].name);
    }
  }

  exit(0);
}
