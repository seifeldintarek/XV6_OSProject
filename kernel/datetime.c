#include "types.h"
#include "riscv.h"
#include "defs.h"
#include "param.h"

uint64
sys_datetime(void)
{
  printf("DateTime syscall called (UTC-2)\n");
  return 0;
}
