#include "include/assembly.hpp"
#include <stdlib.h>

int dummy_leaf_rv = 0;

int FORCE_NOINLINE dummy_leaf_func(int v) {
  dummy_leaf_rv += rand();
  return dummy_leaf_rv;
}


// we know rand is call immediately afterwards
// so get the plt address from ra
#define GET_GOT_LOC(stack_offset)   \
  asm volatile(                     \
    "movl %2, %%ecx;"               \
    "movslq %%ecx, %%rcx;"          \
    "addq %%rsp, %%rcx;"            \
    "mov  (%%rcx), %0;"             \
    "mov  0x1(%0), %1;"             \
    : "+r"(pc), "+r"(offset)        \
    : "r"(stack_offset)             \
    : "rcx"                         \
  );                                \
  pc += 5;                          \
  pc += offset;                     \
  asm volatile(                     \
    "mov 0x2(%0), %%eax;"           \
    "add $0x6, %0;"                 \
    "add %%rax, %0;"                \
    : "+r"(pc) : : "rax"            \
  );

void get_got_func(void **gotp, int stack_offset) {
  char *pc = NULL;
  int offset = 0;

  GET_GOT_LOC(stack_offset)

  *gotp = pc;
}

void replace_got_func(void **fake, void *got) {
  asm volatile(
    "movq %1, (%0);" // replace the GPT entry
    : : "r"(got), "r"(fake)
  );
}

#undef GET_GOT_LOC

