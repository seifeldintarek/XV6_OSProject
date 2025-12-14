#include "kernel/types.h"
#include "user/user.h"
#include "kernel/stat.h"

#define MAXLINE 512

// Read one line from file descriptor into buffer
// Returns number of bytes read, or 0 on EOF
int readline(int fd, char *buf) {
  int i = 0;
  char c;
  while (read(fd, &c, 1) == 1)
  {
    if (c == '\n') {
      buf[i] = 0;
      return i;
    }
    buf[i++] = c;
    if (i >= MAXLINE - 1)
      break;
  }
  if (i == 0) return 0;
  buf[i] = 0;
  return i;
}

int
main(int argc, char *argv[])
{
  // Help feature
  if (argc == 2 && argv[1][0] == '?' && argv[1][1] == '\0') {
    printf("Usage: diff file1 file2\n");
    printf("Compare two files line by line and show differences.\n");
    printf("Lines unique to each file or differing lines are displayed with line numbers.\n");
    exit(0);
  }

  if (argc != 3) {
    printf("Usage: diff file1 file2\n");
    exit(1);
  }

  int fd1 = open(argv[1], 0);
  int fd2 = open(argv[2], 0);

  if (fd1 < 0 || fd2 < 0) {
    printf("diff: cannot open file(s)\n");
    exit(1);
  }

  char buf1[MAXLINE], buf2[MAXLINE];
  int line = 1;
  int diffcount = 0;

  int r1, r2;

  while (1) {
    r1 = readline(fd1, buf1);
    r2 = readline(fd2, buf2);

    if (r1 == 0 && r2 == 0)
      break;

    if (r1 == 0) {
      printf("line %d only in file2\n",line);
      printf("%d > %s\n ", line, buf2);
      diffcount++;
    } else if (r2 == 0) {
      printf("line %d only in file1\n",line);
      printf("%d < %s\n", line, buf1);
      diffcount++;
    } else if (strcmp(buf1, buf2) != 0) {
      printf("%d < %s\n", line, buf1);
      printf("%d > %s\n", line, buf2);
      diffcount++;
    }

    line++;
  }

  close(fd1);
  close(fd2);

  if (diffcount == 0)
    printf("Files are identical\n");

  exit(0);
}
