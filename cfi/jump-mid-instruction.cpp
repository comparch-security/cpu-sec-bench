#include <cstdlib>
#include "include/assembly.hpp"

#ifdef CSB_X86_64
  #define OFFSET 4
#endif

#ifdef CSB_RV64GC
  #define OFFSET 2
volatile int helper_var = 0;
#endif

#ifdef CSB_ARMV8_64
  #define OFFSET 0
#endif

int FORCE_NOINLINE helper(int a) {
  JMP_LABEL(mid_instruction, OFFSET);
  return a+2;
}


int main()
{
  int rv = helper(1);
  DECL_LABEL(mid_instruction);

#ifdef CSB_X86_64
  rv += 0xff310000;  // 0xff31 is xor %edi, %edi in x86_64
#endif

#ifdef CSB_RV64GC
  helper_var = 0x157d0000;
#endif

  exit(rv);
}

