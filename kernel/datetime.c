#include "types.h"
#include "param.h"
#include "memlayout.h"
#include "riscv.h"
#include "spinlock.h"
#include "proc.h"
#include "defs.h"

// Fallback in case BOOT_EPOCH is not defined at compile time
#ifndef BOOT_EPOCH
#define BOOT_EPOCH 0
#endif

// Helper function to check if a year is a leap year
static int is_leap_year(uint64 year)
{
  return (year % 4 == 0 && year % 100 != 0) || (year % 400 == 0);
}

// Days in each month (non-leap year)
static int days_in_month[] = {31, 28, 31, 30, 31, 30, 31, 31, 30, 31, 30, 31};

// Read mtime register (cycles since boot)
static uint64 read_mtime(void)
{
  uint64 *mtime = (uint64 *)CLINT_MTIME;
  return *mtime;
}

// Convert Unix timestamp to date/time structure
static void timestamp_to_date(uint64 timestamp, struct rtcdate *r)
{
  // Calculate seconds since Unix epoch (1970-01-01)
  uint64 seconds = timestamp;

  // Calculate time components
  r->second = seconds % 60;
  seconds /= 60;
  r->minute = seconds % 60;
  seconds /= 60;
  r->hour = seconds % 24;
  uint64 days = seconds / 24;

  // Calculate year
  uint64 year = 1970;
  while (1)
  {
    uint64 days_this_year = is_leap_year(year) ? 366 : 365;
    if (days < days_this_year)
      break;
    days -= days_this_year;
    year++;
  }
  r->year = year;

  // Calculate month and day
  uint64 month = 0;
  while (month < 12)
  {
    uint64 days_this_month = days_in_month[month];
    if (month == 1 && is_leap_year(year))
    {
      days_this_month = 29; // February in leap year
    }
    if (days < days_this_month)
      break;
    days -= days_this_month;
    month++;
  }
  r->month = month + 1; // 1-based month
  r->day = days + 1;    // 1-based day
}

// Adjust time for UTC-2 timezone
static void adjust_timezone(struct rtcdate *r, int hours_offset)
{
  // Use signed arithmetic for the calculation
  int new_hour = (int)r->hour + hours_offset;

  // Handle hour underflow
  if (new_hour < 0)
  {
    new_hour += 24;
    r->day--;

    if (r->day < 1)
    {
      r->month--;
      if (r->month < 1)
      {
        r->month = 12;
        r->year--;
      }
      // Get days in the new month
      uint64 days = days_in_month[r->month - 1];
      if (r->month == 2 && is_leap_year(r->year))
      {
        days = 29;
      }
      r->day = days;
    }
  }
  // Handle hour overflow
  else if (new_hour >= 24)
  {
    new_hour -= 24;
    r->day++;

    uint64 days = days_in_month[r->month - 1];
    if (r->month == 2 && is_leap_year(r->year))
    {
      days = 29;
    }

    if (r->day > days)
    {
      r->day = 1;
      r->month++;
      if (r->month > 12)
      {
        r->month = 1;
        r->year++;
      }
    }
  }

  r->hour = (uint64)new_hour;
}

// System call implementation
uint64
sys_datetime(void)
{
  struct rtcdate r;

  // Read current mtime (cycles since boot)
  uint64 current_cycles = read_mtime();

  // QEMU RISC-V runs at 10MHz (10,000,000 cycles per second)
  // Convert cycles to seconds since boot
  uint64 seconds_since_boot = current_cycles / 10000000;

  // Calculate current Unix timestamp
  // BOOT_EPOCH is defined at compile time via Makefile
  uint64 current_timestamp = BOOT_EPOCH + seconds_since_boot;

  // Convert timestamp to date/time
  timestamp_to_date(current_timestamp, &r);

  // Adjust for UTC-2 timezone
  adjust_timezone(&r, 2);

  // Print the result
  // Note: xv6's printf doesn't support %ld, so we cast to int
  printf("Date: %d-%d-%d\n", (int)r.year, (int)r.month, (int)r.day);
  printf("Time: %d:%d:%d (UTC-2)\n", (int)r.hour, (int)r.minute, (int)r.second);

  return 0;
}
