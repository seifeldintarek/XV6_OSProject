#include "kernel/types.h"
#include "kernel/stat.h"
#include "kernel/fcntl.h"
#include "user/user.h"

#define FLAG_LINES 1
#define FLAG_WORDS 2
#define FLAG_CHARS 4
#define FLAG_LLONG 8

char buf[512];

void wc(int fd, char *name, int flags,
        int *tlines, int *twords, int *tchars, int *tllong)
{
  int i, n;
  int l = 0, w = 0, c = 0;
  int inword = 0;
  int longest = 0, currentLen = 0;

  while ((n = read(fd, buf, sizeof(buf))) > 0)
  {
    for (i = 0; i < n; i++)
    {
      c++;
      if (buf[i] == '\n')
      {
        l++;
        if (currentLen > longest)
          longest = currentLen;
        currentLen = 0;
      }
      else
      {
        currentLen++;
      }

      if (strchr(" \r\t\n\v", buf[i]))
        inword = 0;
      else if (!inword)
      {
        w++;
        inword = 1;
      }
    }
  }

  if (currentLen > longest)
    longest = currentLen;

  *tlines += l;
  *twords += w;
  *tchars += c;
  if (longest > *tllong)
    *tllong = longest;

  if (flags == 0)
    flags = FLAG_LINES | FLAG_WORDS | FLAG_CHARS;

  if (flags & FLAG_LINES)
    printf("%d ", l);
  if (flags & FLAG_WORDS)
    printf("%d ", w);
  if (flags & FLAG_CHARS)
    printf("%d ", c);
  if (flags & FLAG_LLONG)
    printf("%d ", longest);

  printf("%s\n", name);
}

int main(int argc, char *argv[])
{
  int fd, i = 1;
  int flags = 0;
  int files = 0;
  int total_lines = 0, total_words = 0, total_chars = 0, total_llong = 0;

  if (argc > 1 && argv[1][0] == '-')
  {
    char *opt = argv[1] + 1;
    while (*opt)
    {
      if (*opt == 'l')
        flags |= FLAG_LINES;
      else if (*opt == 'w')
        flags |= FLAG_WORDS;
      else if (*opt == 'c')
        flags |= FLAG_CHARS;
      else if (*opt == 'L')
        flags |= FLAG_LLONG;
      opt++;
    }
    i++;
  }

  if (i >= argc)
  {
    wc(0, "", flags, &total_lines, &total_words, &total_chars, &total_llong);
    exit(0);
  }

  for (; i < argc; i++)
  {
    if ((fd = open(argv[i], O_RDONLY)) < 0)
    {
      printf("wc: cannot open %s\n", argv[i]);
      exit(1);
    }
    wc(fd, argv[i], flags,
       &total_lines, &total_words, &total_chars, &total_llong);
    files++;
    close(fd);
  }

  if (files > 1)
  {
    if (flags == 0)
      flags = FLAG_LINES | FLAG_WORDS | FLAG_CHARS;
    printf("\n");
    if (flags & FLAG_LINES)
      printf("%d ", total_lines);
    if (flags & FLAG_WORDS)
      printf("%d ", total_words);
    if (flags & FLAG_CHARS)
      printf("%d ", total_chars);
    if (flags & FLAG_LLONG)
      printf("%d ", total_llong);
    printf("total\n");
  }

  exit(0);
}
