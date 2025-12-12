#include "kernel/types.h"
#include "user/user.h"

int main() {
  int count1 = count();
  printf("syscalls invoked: %d\n", count1);

  exit(0);
}
