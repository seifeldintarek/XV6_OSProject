#include "kernel/types.h"
#include "user/user.h"

int main() {
  int rand_num = random();
  printf("Random number: %d\n", rand_num);
  exit(0);
}
