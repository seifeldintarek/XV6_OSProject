#include "kernel/types.h"
#include "kernel/stat.h"
#include "user/user.h"

// CPU-intensive work
void cpu_work(int id, int iterations)
{
    int i, j;
    volatile int x = 0;
    int last_tick = uptime();

    printf("[P%d | PID %d] START at tick %d\n", id, getpid(), last_tick);

    for (i = 0; i < iterations; i++) {
        for (j = 0; j < 500000; j++) {
            x += 1;
        }

        if (i % 50 == 0) {
            int now = uptime();
            printf("[P%d | PID %d] iter=%d  tick=%d  (+%d ticks)\n",
                   id, getpid(), i, now, now - last_tick);
            last_tick = now;
        }
    }

    printf("[P%d | PID %d] DONE at tick %d\n\n",
           id, getpid(), uptime());
}

int main(void)
{
    printf("\n=== Starvation Test (Newer = Higher Priority) ===\n");
    printf("Legend: whoever prints is CURRENTLY on CPU\n\n");

    int pid1 = fork();
    if (pid1 == 0) {
        printf("[P1 | PID %d] OLDEST process\n", getpid());
        cpu_work(1, 300);
        exit(0);
    }

    sleep(1);  // let P1 age

    for (int i = 2; i <= 5; i++) {
        int pid = fork();
        if (pid == 0) {
            printf("[P%d | PID %d] NEWER process\n", i, getpid());
            cpu_work(i, 150);
            exit(0);
        }
        sleep(1);
    }

    printf("\n--- All processes created ---\n");
    printf("Expected finish order: P5 → P4 → P3 → P2 → P1\n\n");

    for (int i = 0; i < 5; i++)
        wait(0);

    printf("\n=== Test Complete ===\n");
    exit(0);
}
