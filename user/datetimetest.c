#include "kernel/types.h"
#include "kernel/stat.h"
#include "user/user.h"

int main(int argc, char *argv[])
{
  printf("Current DateTime (UTC-2):\n");
  datetime();
  exit(0);
}
