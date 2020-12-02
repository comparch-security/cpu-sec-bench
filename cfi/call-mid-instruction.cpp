#include <cstdlib>
#include "include/assembly.hpp"

#ifdef __x86_64
  #define OFFSET 4
#endif

#if defined(__riscv) && __riscv_xlen == 64
  #define OFFSET 2
volatile int helper_var = 0;
#endif

int FORCE_NOINLINE helper(int a) {
  CALL_LABEL(mid_instruction, OFFSET);
  return a+2;
}

int main()
{
  int rv = helper(1);
  DECL_LABEL(mid_instruction);

#ifdef __x86_64
  rv += 0xff310000;  // 0xff31 is xor %edi, %edi in x86_64
#endif

#if defined(__riscv) && __riscv_xlen == 64
  helper_var = 0x157d0000;  // 0x157d is addi a0,a0 -1 in RV64C
#endif
  exit(rv);
}
