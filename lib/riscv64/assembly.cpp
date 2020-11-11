#include "include/assembly.hpp"
#include <stdlib.h>

int dummy_leaf_rv = 0;

int FORCE_NOINLINE dummy_leaf_func(int v) {
  dummy_leaf_rv += rand();
  return dummy_leaf_rv;
}

void replace_got_func(void **org, void **fake) {
  asm volatile(
	"lwu  t0, (a0);"
	"srai t0, t0, 12;"  // get 20 u-imm in t0
	"slli t0, t0, 12;"
	"add  t1, a0, t0;"  // get the pc + (imm << 12) for auipc
	"lwu  t0, 4(a0);;"
	"srai t0, t0, 20;"  // get the lower [11:0] offset
	"add  t1, t1, t0;"  // get the GOT address
	"sd   a1, 0(t1);"   // replace the GOT entry
  );
}
