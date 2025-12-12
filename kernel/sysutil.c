#include "types.h"

extern int keyboard_int_cnt;
uint64 sys_kbdint()
{
  return keyboard_int_cnt;
}
uint64 sys_shutdown()
{

*(volatile uint32 *) 0x100000 = 0x5555;
return 0;
}
uint seed;    // static so it keeps value between calls
extern uint ticks;

uint64
sys_random(void)
{
    uint a = 1103515245;
    uint c = 12345;

   seed=ticks;

    seed = (a * seed + c) % 10;  // 31-bit pseudo-random
    return seed;
}
