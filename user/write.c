#include "kernel/types.h"
#include "kernel/stat.h"
#include "kernel/fcntl.h"
#include "user/user.h"

char buf[512];

int main(int argc, char *argv[]) {
    if(argc != 2){
        printf("Usage: nano filename\n");
        exit(1);
    }

    char *filename = argv[1];
    int fd = open(filename, O_CREATE | O_RDWR);
    if(fd < 0){
        printf("nano: cannot open %s\n", filename);
        exit(1);
    }

    int n;

    printf("Simple nano editor. Type lines and end with Ctrl+D\n");

    // Read and display existing file content
    while((n = read(fd, buf, sizeof(buf))) > 0){
        write(1, buf, n);
    }

    // xv6 doesn’t have lseek, so just close & reopen for writing at the end
    close(fd);
    fd = open(filename, O_CREATE | O_WRONLY);
    if(fd < 0){
        printf("nano: cannot reopen %s\n", filename);
        exit(1);
    }

    // Read from stdin and write to file
    while((n = read(0, buf, sizeof(buf))) > 0){
        if(write(fd, buf, n) != n){
            printf("nano: write error\n");
            break;
        }
    }

    close(fd);
    printf("\nSaved %s\n", filename);
    exit(0);
}
