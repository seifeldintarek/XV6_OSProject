#include "kernel/types.h"
#include "kernel/stat.h"
#include "user/user.h"

int
main(int argc, char *argv[])
{
  if (argc != 2) {
    fprintf(2, "Usage: sleep <ticks>\n");
    exit(1);
  }

  int ticks = atoi(argv[1]);
  if (ticks < 0) {
    fprintf(2, "sleep: number of ticks must be positive\n");
    exit(1);
  }

  sleep(ticks); // call the system call
  exit(0);
}