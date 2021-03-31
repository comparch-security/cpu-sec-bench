#include "include/assembly.hpp"
#include <stdlib.h>

int dummy_leaf_rv = 0;

int FORCE_NOINLINE dummy_leaf_func(int v) {
  return v + rand();
}

#define GET_GOT_LOC                             \
   asm volatile(                                \
    "addi %0, ra, 0;"                           \
    "lwu  %1, 0(ra);"                           \
    : "+r"(pc), "+r"(inst)                      \
  );                                            \
  offset += ((inst >> 31) & 0x1)   << 20;       \
  offset += ((inst >> 21) & 0x3ff) << 1;        \
  offset += ((inst >> 20) & 0x1)   << 11;       \
  offset += ((inst >> 12) & 0xff)  << 12;       \
  if(offset >> 20) {                            \
    offset |= 0xfff00000;                       \
    offset = ~offset + 1;                       \
    pc -= offset;                               \
  } else                                        \
    pc += offset;                               \
  asm volatile(                                 \
    "lwu  t0, (%0);"                            \
    "srai t0, t0, 12;"                          \
    "slli t0, t0, 12;"                          \
    "add  t1, %0, t0;"                          \
    "lwu  t0, 4(%0);"                           \
    "srai t0, t0, 20;"                          \
    "add  %0, t1, t0;"                          \
    : "+r"(pc)                                  \
  );                                            \

void get_got_func(void **gotp) {
  char *pc = NULL;
  unsigned int offset = 0, inst = 0;

  GET_GOT_LOC
 
  *gotp = pc;
}

void replace_got_func(void **fake) {
  char *pc = NULL;
  unsigned int offset = 0, inst = 0;

  GET_GOT_LOC
 
  asm volatile(
    "sd   %0, 0(%1);"
    : : "r"(fake), "r"(pc)
  );
}

#undef GET_GOT_LOC
