#include <cstdlib>
#include "include/assembly.hpp"

int stack_offset = 0;

void FORCE_NOINLINE helper()
{
  unsigned long long m = 0;
  ENFORCE_NON_LEAF_FUNC;
  MOD_STACK_DAT(exit, stack_offset);
  PASS_INT_ARG(0, m);
}

int main(int argc, char* argv[])
{
  // get the offset of RA on stack
  stack_offset = 8 * (argv[1][0] - '0');

  helper();
  return 3;
}
