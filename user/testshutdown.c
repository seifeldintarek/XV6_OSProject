#include "kernel/types.h"
#include "user/user.h"

int main() {
  printf("Shutting down ...");

  shutdown();



  exit(0);
}
