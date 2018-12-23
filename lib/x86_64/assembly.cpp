#include "include/assembly.hpp"

unsigned long long min_stack_size = 0;
unsigned long long helper_sp = 0;

void FORCE_NOINLINE asm_stack_test_helper() {
  asm volatile("movq %%rsp, %0;" : "=r"(helper_sp) : : );
}

void asm_stack_test() {
  asm volatile("movq %%rsp, %0;" : "=r"(min_stack_size) : : );
  asm_stack_test_helper();
  min_stack_size -= (helper_sp + 8);
}

void replace_got_func(void **org, void **fake) {
  asm volatile(
    "mov 0x2(%rdi), %eax;"
    "add $0x6, %rdi;"
    "add %rax, %rdi;"
    "movq %rsi, (%rdi);"
  );
}
