#include "kernel/types.h"
#include "kernel/stat.h"
#include "user/user.h"
#include "kernel/fs.h"
#include "kernel/fcntl.h"

int main(int argc, char *argv[]) {

    // Help feature
    if (argc == 2 && argv[1][0] == '?' && argv[ 1][1] == '\0') {
        printf("Usage: touch <filename>\n");
        printf("Create an empty file with the specified filename.\n");
        printf("If the file already exists, an error is reported.\n");
        exit(0);
    }

    if (argc != 2) {
        printf("Usage: touch <filename>\n");
        exit(1);
    }

    char *filename = argv[1];

    // Try to open file without creating
    int fd = open(filename, O_WRONLY);
    if (fd >= 0) {
        // File exists
        printf("Error: file '%s' already exists\n", filename);
        close(fd);
        exit(1);
    }

    // File does not exist → create it
    fd = open(filename, O_CREATE | O_WRONLY);
    if (fd < 0) {
        printf("Error: cannot create file '%s'\n", filename);
        exit(1);
    }

    close(fd);
    exit(0);
}
