#include "kernel/types.h"
#include "kernel/stat.h"
#include "user/user.h"

#define MAX_LINES 20
#define MAX_LINE 512

char lines[MAX_LINES][MAX_LINE];

void tail(int fd, int n)
{
    char buf[1];
    int total = 0;
    int pos = 0;
    
    // Read file line by line into circular buffer
    while (read(fd, buf, 1) > 0) {
        if (buf[0] == '\n') {
            lines[total % MAX_LINES][pos] = '\0';
            total++;
            pos = 0;
        } else if (pos < MAX_LINE - 1) {
            lines[total % MAX_LINES][pos++] = buf[0];
        }
    }
    
    // Handle last line without newline
    if (pos > 0) {
        lines[total % MAX_LINES][pos] = '\0';
        total++;
    }
    
    // Print last n lines
    int start = total > n ? total - n : 0;
    for (int i = start; i < total; i++) {
        printf("%s\n", lines[i % MAX_LINES]);
    }
}

int main(int argc, char *argv[])
{
    int n = 10;
    int fd = 0;
    
    // Check for -n flag
    if (argc > 2 && argv[1][0] == '-' && argv[1][1] == 'n') {
        n = atoi(argv[2]);
        argc -= 2;
        argv += 2;
    }
    
    // Open file if provided, else use stdin
    if (argc > 1) {
        fd = open(argv[1], 0);
        if (fd < 0) {
            printf("tail: cannot open %s\n", argv[1]);
            exit(1);
        }
    }
    
    tail(fd, n);
    
    if (fd > 0) {
        close(fd);
    }
    
    exit(0);
}
