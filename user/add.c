#include "kernel/types.h"
#include "kernel/stat.h"
#include "user/user.h"

int
main(int argc, char *argv[])
{
    // Help triggers
    if (argc == 2 &&
       (!strcmp(argv[1], "?") ||
        !strcmp(argv[1], "-h") ||
        !strcmp(argv[1], "--help"))) {

        printf("Usage: add <num1> <num2>\n");
        printf("Adds two integers (positive or negative).\n");
        printf("\nExamples:\n");
        printf("  add 3 4      # output: 7\n");
        printf("  add -5 10    # output: 5\n");
        printf("  add ?        # show this help\n");
        exit(0);
    }

    // Must have 2 args
    if (argc != 3) {
        printf("add uses three args\n");
        exit(1);
    }

    // Input validation: check if first char is alphabetic
    if ((argv[1][0] >= 'a' && argv[1][0] <= 'z') ||
        (argv[1][0] >= 'A' && argv[1][0] <= 'Z')) {
        exit(3);
    }

    int n1, n2;

    // Handle negative sign
    if (argv[1][0] == '-') {
        argv[1][0] = '0';
        n1 = atoi(argv[1]);
        n1 = -n1;
    } else {
        n1 = atoi(argv[1]);
    }

    if (argv[2][0] == '-') {
        argv[2][0] = '0';
        n2 = atoi(argv[2]);
        n2 = -n2;
    } else {
        n2 = atoi(argv[2]);
    }

    int sum = n1 + n2;

    printf("%d\n", sum);

    exit(0);
}
