#include <cstdlib>
#include "include/assembly.hpp"

#ifdef CSB_X86_64
  #define OFFSET 3
#endif

#ifdef CSB_RV64GC
  #define OFFSET 2
volatile int helper_var = 0;
#endif

#ifdef CSB_ARMV8_64
  #define OFFSET 0
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

#ifdef CSB_X86_64
  rv = 0xff310000;  // 0xff31 is xor %edi, %edi in x86_64
#endif

#ifdef CSB_RV64GC
  helper_var = 0x45010000;
#endif

#ifdef CSB_ARMV8_64
  rv = rv ? 1 : 0;  // it is not easy to form a mid-instruction on ARM v8 yet
#endif

  exit(rv);
}
