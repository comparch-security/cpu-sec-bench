#include <cstdlib>
#include "include/assembly.hpp"

#ifdef __x86_64
  #define OFFSET 4
#endif

#if defined(__riscv) && __riscv_xlen == 64
  #define OFFSET 0
#endif

int FORCE_NOINLINE helper(int a) {
  ENFORCE_NON_LEAF_FUNC;
  unsigned long long t = 0;
  LOAD_LABEL(mid_instruction, t);
  MOD_RET_DAT(t+OFFSET);
  return a+2;
}

int main()
{
  int rv = helper(1);
  DECL_LABEL(mid_instruction);
#ifdef __x86_64
  rv += 0xff310000;  // 0xff31 is xor %edi, %edi in x86_64
#endif
  exit(rv);
}
