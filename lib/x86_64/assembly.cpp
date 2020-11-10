#include "include/assembly.hpp"
#include <stdlib.h>

int dummy_leaf_rv = 0;

int FORCE_NOINLINE dummy_leaf_func(int v) {
  dummy_leaf_rv += rand();
  return dummy_leaf_rv;
}

void replace_got_func(void **org, void **fake) {
  asm volatile(
    "mov 0x2(%rdi), %eax;"  // get the pc related offset
    "add $0x6, %rdi;"       // get pc_next
    "add %rax, %rdi;"       // get the GOT address
    "movq %rsi, (%rdi);"    // replace the GPT entry
  );
}
