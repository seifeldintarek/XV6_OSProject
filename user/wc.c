#include "kernel/types.h"
#include "kernel/stat.h"
#include "kernel/fcntl.h"
#include "user/user.h"

char buf[512];

void
wc(int fd, char *name, int show_lines, int show_words, int show_chars, int show_longest,
   int *total_lines, int *total_words, int *total_chars, int *total_longest)
{
  int i, n;
  int lines=0, words=0, chars=0;
  int inword=0;
  int longest=0, curlen=0;

  while((n = read(fd, buf, sizeof(buf))) > 0){
    for(i=0;i<n;i++){
      chars++;
      if(buf[i]=='\n'){
        lines++;
        if(curlen>longest) longest=curlen;
        curlen=0;
      } else curlen++;

      if(buf[i]==' '||buf[i]=='\r'||buf[i]=='\t'||buf[i]=='\n'||buf[i]=='\v')
        inword=0;
      else if(!inword){words++; inword=1;}
    }
  }

  if(curlen>longest) longest=curlen;

  // accumulate totals
  *total_lines += lines;
  *total_words += words;
  *total_chars += chars;
  if(longest > *total_longest) *total_longest = longest;

  // default show all if none selected
  if(!show_lines && !show_words && !show_chars && !show_longest){
    show_lines = 1; show_words = 1; show_chars = 1;show_longest=1;
  }

  if(show_lines) printf("%d ", lines);
  if(show_words) printf("%d ", words);
  if(show_chars) printf("%d ", chars);
  if(show_longest) printf("%d ", longest);
  printf("%s\n", name);
}

int
main(int argc, char *argv[])
{
  int i=1, fd;
  int show_lines=0, show_words=0, show_chars=0, show_longest=0;
  int total_lines=0, total_words=0, total_chars=0, total_longest=0;
  int files=0;

  // parse flags directly
  if(argc>1 && argv[1][0]=='-'){
    char c;
    for(i=1; (c=argv[1][i])!='\0'; i++){
      if(c=='l') show_lines=1;
      else if(c=='w') show_words=1;
      else if(c=='c') show_chars=1;
      else if(c=='L') show_longest=1;

    }
    i=2; // first file argument
  } else i=1;//i fdlt b 1 m3ndish esm file

  // no files -> stdin
  if(i>=argc){
    printf("No files given\n");
    exit(0);

  }

  for(; i<argc; i++){
    if((fd=open(argv[i], O_RDONLY))<0){
      printf("wc: cannot open %s\n", argv[i]);
      exit(1);
    }

    wc(fd, argv[i], show_lines, show_words, show_chars, show_longest,
       &total_lines, &total_words, &total_chars, &total_longest);
    close(fd);
    files++;
  }

  // totals for multiple files
  if(files>1){
    if(!show_lines && !show_words && !show_chars && !show_longest){
      show_lines=1; show_words=1; show_chars=1;
    }
    if(show_lines) printf("%d ", total_lines);
    if(show_words) printf("%d ", total_words);
    if(show_chars) printf("%d ", total_chars);
    if(show_longest) printf("%d ", total_longest);
    printf("total\n");
  }

  exit(0);
}
