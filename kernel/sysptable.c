#include "types.h"
#include "riscv.h"
#include "param.h"
#include "memlayout.h"
#include "spinlock.h"
#include "proc.h"
#include "defs.h"

uint64
sys_getptable(void)
{
  int nproc;
  uint64 buffer;

  // Get arguments from user space
  argint(0, &nproc);
  argaddr(1, &buffer);

  return getptable(nproc, buffer); // bta5od el user arguments w bteb3atha lel kernel function w byerga3laha 0 aw 1
}
