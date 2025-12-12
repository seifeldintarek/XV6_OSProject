typedef unsigned int uint;
typedef unsigned short ushort;
typedef unsigned char uchar;

typedef unsigned char uint8;
typedef unsigned short uint16;
typedef unsigned int uint32;
typedef unsigned long uint64;

typedef uint64 pde_t;

struct rtcdate
{
  uint64 second;
  uint64 minute;
  uint64 hour;
  uint64 day;
  uint64 month;
  uint64 year;
};
