#include "include/assembly.hpp"
#include <stdlib.h>

int dummy_leaf_rv = 0;

int FORCE_NOINLINE dummy_leaf_func(int v) {
  dummy_leaf_rv += rand();
  return dummy_leaf_rv;
}


// we know rand is call immediately afterwards
// so get the plt address from ra
#define GET_GOT_LOC                 \
  asm volatile(                     \
    "mov  (%%rsp), %0;"             \
    "mov  0x1(%0), %1;"             \
    : "+r"(pc), "+r"(offset)        \
  );                                \
  pc += 5;                          \
  pc += offset;                     \
  asm volatile(                     \
    "mov 0x2(%0), %%eax;"           \
    "add $0x6, %0;"                 \
    "add %%rax, %0;"                \
    : "+r"(pc) : : "rax"            \
  );

void get_got_func(void **gotp) {
  char *pc = NULL;
  int offset = 0;

  GET_GOT_LOC

  *gotp = pc;
}

void replace_got_func(void **fake) {
  
  char *pc = NULL;
  int offset = 0;

  GET_GOT_LOC

  asm volatile(
    "movq %1, (%0);" // replace the GPT entry
    : : "r"(pc), "r"(fake) : "rax"
  );
}

#undef GET_GOT_LOC

