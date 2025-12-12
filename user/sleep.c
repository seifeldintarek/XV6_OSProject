#include "kernel/types.h"
#include "kernel/stat.h"
#include "user/user.h"

int
main(int argc, char *argv[])
{
  // Help triggers
  if (argc == 2 &&
     (!strcmp(argv[1], "--help") ||
      !strcmp(argv[1], "-h") ||
      !strcmp(argv[1], "?"))) {

    printf("Usage: sleep <ticks>\n");
    printf("Pause execution for a number of clock ticks.\n");
    printf("\nExamples:\n");
    printf("  sleep 10    # sleep for 10 ticks\n");
    printf("  sleep 100   # sleep for 100 ticks\n");
    printf("  sleep ?     # show this help message\n");
    exit(0);
  }

  if (argc != 2) {
    fprintf(2, "Usage: sleep <ticks>\n");
    exit(1);
  }

  int ticks = atoi(argv[1]);
  if (ticks < 0) {
    fprintf(2, "sleep: number of ticks must be positive\n");
    exit(1);
  }

  sleep(ticks);
  exit(0);
}
