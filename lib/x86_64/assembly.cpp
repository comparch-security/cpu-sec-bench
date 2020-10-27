#include "include/assembly.hpp"

unsigned long long min_stack_size = 8;
int dummy_leaf_rv = 0;

int FORCE_NOINLINE dummy_leaf_func(int v) {
  return (1 + v + dummy_leaf_rv);
}

void replace_got_func(void **org, void **fake) {
  asm volatile(
    "mov 0x2(%rdi), %eax;"
    "add $0x6, %rdi;"
    "add %rax, %rdi;"
    "movq %rsi, (%rdi);"
  );
}
