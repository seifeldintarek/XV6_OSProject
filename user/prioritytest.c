#include "kernel/types.h"
#include "kernel/stat.h"
#include "user/user.h"

void
cpu_work(int id, int iterations)
{
    int i, j;
    volatile int x = 0;

    for(i = 0; i < iterations; i++){
        for(j = 0; j < 100000; j++){
            x += 1;
        }
        if(i % 25 == 0){
            printf("[P%d] iter %d\n", id, i);
        }
    }
    printf("[P%d] *** DONE ***\n", id);
}

int
main(void)
{

    // Set parent to very low priority
    setpriority(getpid(), 99999);
    printf("Parent priority set to 99999\n\n");

    // Fork ALL processes immediately (no sleep!)
    int pid1 = fork();
    if(pid1 == 0){
        setpriority(getpid(), 5000);
        printf("P1 (PID %d): priority=5000 (LONG)\n", getpid());
        cpu_work(1, 300);
        exit(0);
    }

    int pid2 = fork();
    if(pid2 == 0){
        setpriority(getpid(), 1000);
        printf("P2 (PID %d): priority=1000 (MEDIUM)\n", getpid());
        cpu_work(2, 200);
        exit(0);
    }

    int pid3 = fork();
    if(pid3 == 0){
        setpriority(getpid(), 200);
        printf("P3 (PID %d): priority=200 (SHORT)\n", getpid());
        cpu_work(3, 100);
        exit(0);
    }

    printf("--- All processes ready ---\n");
    printf("Expected: P3 finishes first, then P2, then P1\n\n");

    wait(0);
    wait(0);
    wait(0);

    printf("\n=== Test Complete ===\n");
    exit(0);
}
