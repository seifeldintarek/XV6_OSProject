#include "kernel/types.h"
#include "kernel/stat.h"
#include "user/user.h"

int
main(int argc, char *argv[])
{
   if (argc != 3) {
        printf("add uses three args\n");
        exit(1);
    }
if(argv[1][0] >= 'a' || argv[1][0] >= 'z')
  {
      exit(3);
  }

  if(argv[1][0] >= 'A' || argv[1][0] >= 'Z')
  {
      exit(3);
  }

int n1,n2;

  if(argv[1][0] == '-')
  {
    argv[1][0] = '0';
    n1=atoi(argv[1]);
    n1=-n1;
  }
  else n1=atoi(argv[1]);

  if (argv[2][0] == '-')
  {
    argv[2][0] = '0';
    n2=atoi(argv[2]);
    n2=-n2;
  }
  else
  n2=atoi(argv[2]);

  int sum=n1+n2;

  printf("%d\n",sum);

exit(0);




}