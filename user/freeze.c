#include "kernel/types.h"
#include "kernel/stat.h"
#include "user/user.h"

int main(int argc, char *args[]){
  // Help feature
  if(argc == 2 && args[1][0] == '?' && args[1][1] == '\0'){
    printf("Usage: sleep <seconds>\n");
    printf("Pause execution for the specified number of seconds.\n");
    exit(0);
  }

  if(argc != 2){
    printf("Usage: sleep <seconds>\n");
    exit(1);
  }

  int period = atoi(args[1]);

  if(period > 0){
    sleep(period);
  } else {
    printf("period not valid\n");
    exit(1);
  }

  exit(0);
}
