#include "include/assembly.hpp"
#include <stdlib.h>

int dummy_leaf_rv = 0;

int FORCE_NOINLINE dummy_leaf_func(int v) {
  dummy_leaf_rv += rand();
  return dummy_leaf_rv;
}


// we know rand is call immediately afterwards
// so get the plt address from ra
#define GET_GOT_LOC(offset)         \
  asm volatile(                     \
    "mov 0x2(%0), %%eax;"           \
    "add $0x6, %0;"                 \
    "add %%rax, %0;"                \
    : "+r"(pc) : : "rax"            \
  );

void get_got_func(void **gotp, void *label, int cet) {
  char *pc = (char *)label;
  if(cet) pc += 4; //bypass endbr64
  int offset = *(int *)(pc+1);
  pc += 5 + offset; // point to 1st instruction in plt

  if(cet) {
    pc += 4; // bypass endbr64
    offset = *(int *)(pc+3);
    pc += 7 + offset;
  } else {
    offset = *(int *)(pc+2);
    pc += 6 + offset;
  }

  *gotp = pc;
}

void replace_got_func(void **fake, void *got) {
  asm volatile(
    "movq %1, (%0);" // replace the GPT entry
    : : "r"(got), "r"(fake)
  );
}

#undef GET_GOT_LOC

