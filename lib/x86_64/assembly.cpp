#include "include/assembly.hpp"
#include <stdlib.h>

int dummy_leaf_rv = 0;

int FORCE_NOINLINE dummy_leaf_func(int v) {
  dummy_leaf_rv += rand();
  return dummy_leaf_rv;
}

void replace_got_func(void **fake) {
  char *pc = NULL;
  int offset = 0;

  // we know rand is call immediately afterwards
  // so get the plt address from ra
  asm volatile(
    "mov  (%%rsp), %0;"
    "mov  0x1(%0), %1;"
    : "+r"(pc), "+r"(offset)
  );

  pc += 5;
  pc += offset;

  asm volatile(
    "mov 0x2(%0), %%eax;"   // get the pc related offset
    "add $0x6, %0;"         // get pc_next
    "add %%rax, %0;"        // get the GOT address
    "movq %1, (%0);"        // replace the GPT entry
    : : "r"(pc), "r"(fake) : "rax"
  );
}
