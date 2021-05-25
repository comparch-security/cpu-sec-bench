#include <cstdlib>
#include "include/assembly.hpp"

static volatile int grv = 1;
int stack_offset = 0;

void FORCE_NOINLINE helper() {
  ENFORCE_NON_LEAF_FUNC;
  grv = 3;

  MOD_STACK_LABEL(helper2_ret, stack_offset);
  grv = 0;
}

void FORCE_NOINLINE helper2() {
  grv = 4;
}

int main(int argc, char* argv[])
{
  // get the offset of RA on stack
  stack_offset = 8 * (argv[1][0] - '0');

  // call a function but illegally return
  helper();
  helper2();// failed if runs here
  // the elligal return site
  DECL_LABEL(helper2_ret);

  exit(grv);
}
