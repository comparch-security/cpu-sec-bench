#include <cstdlib>
#include "include/assembly.hpp"

#ifdef __x86_64
  #define OFFSET 4
#endif

int FORCE_NOINLINE helper(int a) {
  JMP_LABEL(mid_instruction, OFFSET);
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

