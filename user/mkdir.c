#include "kernel/types.h"
#include "kernel/stat.h"
#include "user/user.h"

int
main(int argc, char *argv[])
{
  int i;

  // Help feature
  if(argc == 2 && argv[1][0] == '?' && argv[1][1] == '\0'){
    printf("Usage: mkdir files...\n");
    printf("Create one or more directories specified by the arguments.\n");
    printf("Example: mkdir dir1 dir2\n");
    exit(0);
  }

  if(argc < 2){
    printf("Usage: mkdir files...\n");
    exit(1);
  }

  for(i = 1; i < argc; i++){
    if(mkdir(argv[i]) < 0){
      printf("mkdir: %s failed to create\n", argv[i]);
      break;
    }
  }

  exit(0);
}
