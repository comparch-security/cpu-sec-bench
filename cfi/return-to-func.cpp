#include <cstdlib>
#include "include/assembly.hpp"

int stack_offset = 0;

int FORCE_NOINLINE fake_ret() {
  exit(0);
}

void FORCE_NOINLINE helper() {
  ENFORCE_NON_LEAF_FUNC;
  MOD_STACK_DAT(fake_ret, stack_offset);
}

int main(int argc, char* argv[])
{
  // get the offset of RA on stack
  stack_offset = 8 * (argv[1][0] - '0' + 1);

  helper();
  return 1;
}
