#include "kernel/types.h"
#include "kernel/stat.h"
#include "user/user.h"

int main(int argc, char *argv[]) {
  // Help feature
  if (argc == 2 && argv[1][0] == '?' && argv[1][1] == '\0') {
      printf("Usage: factorial n\n");
      printf("Calculate the factorial of a positive integer n.\n");
      printf("Example: factorial 5  →  outputs 120\n");
      exit(0);
  }

  if (argc != 2)
  {
    exit(1);//Error
  }

  int n = atoi(argv[1]);
  int fact = 1;
  int i = 1;

  if(argv[1][0] >= 'a' || argv[1][0] >= 'z')
  {
      exit(3);//Invalid Input
  }

  if(argv[1][0] >= 'A' || argv[1][0] >= 'Z')
  {
      exit(3);
  }

  if (argv[1][0] == '-')
   {
      exit(1);
   }

  if (n == 0)
  {
    printf("1\n");
    exit(0);
  }
  else if (n > 0)
  {
    while (i <= n)
    {
      fact = fact * i;
      i++;
    }

    printf("%d\n", fact);
    exit(0);
  }
  else
    exit(3);
}
