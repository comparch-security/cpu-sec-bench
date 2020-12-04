#include "include/assembly.hpp"
#include <stdlib.h>

int dummy_leaf_rv = 0;

int FORCE_NOINLINE dummy_leaf_func(int v) {
  return v + rand();
}

void replace_got_func(void **org, void **fake) {
  char *pc = NULL;
  unsigned int offset = 0, inst = 0;

  // we know rand is call immediately afterwards
  // so get the plt address from ra
  asm volatile(
    "addi %0, ra, 0;"
    "lwu  %1, 0(ra);"
    : "+r"(pc), "+r"(inst)
  );

  // get the offset from the JAL instruction
  offset += ((inst >> 31) & 0x1)   << 20;
  offset += ((inst >> 21) & 0x3ff) << 1;
  offset += ((inst >> 20) & 0x1)   << 11;
  offset += ((inst >> 12) & 0xff)  << 12;

  // handle negative offset
  if(offset >> 20) {
    offset |= 0xfff00000;
    offset = ~offset + 1;
    pc -= offset;
  } else
    pc += offset;
 
  asm volatile(
    "lwu  t0, (%1);"
    "srai t0, t0, 12;" // get 20 u-imm
    "slli t0, t0, 12;"
    "add  t1, %1, t0;" // get the pc + (imm << 12)
    "lwu  t0, 4(%1);"
    "srai t0, t0, 20;" // get the lower [11:0]
    "add  t1, t1, t0;" // get the GOT address
    "sd   %0, 0(t1);"
    : : "r"(fake), "r"(pc)
  );
}

