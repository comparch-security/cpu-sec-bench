#include <cstdlib>
#include "include/assembly.hpp"

#ifdef __x86_64
  #define OFFSET 4
#endif

#if defined(__riscv) && __riscv_xlen == 64
  #define OFFSET 2
volatile int helper_var = 0;
#endif

unsigned long long FORCE_NOINLINE helper(unsigned long long a) {
  ENFORCE_NON_LEAF_FUNC_VAR(a);
  MOD_RET_DAT(a);
  return a+2;
}

int main()
{
  unsigned long long rv = 0;
  LOAD_LABEL(mid_instruction, rv);      
  rv = helper(rv+OFFSET);
  DECL_LABEL(mid_instruction);

#ifdef __x86_64
  rv += 0xff310000;  // 0xff31 is xor %edi, %edi in x86_64
#endif

#if defined(__riscv) && __riscv_xlen == 64
  helper_var = 0x45010000;
#endif

  exit(rv);
}
