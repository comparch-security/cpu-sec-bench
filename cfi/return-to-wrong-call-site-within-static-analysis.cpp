#include <cstdlib>
#include "include/assembly.hpp"

static volatile int grv = 0;
int stack_offset = 0;

void FORCE_NOINLINE helper() {
  ENFORCE_NON_LEAF_FUNC;
  grv++;

  if(grv == 2) {
    MOD_STACK_LABEL(helper2_ret, stack_offset);
    grv = 0;
  }
}

int main(int argc, char* argv[])
{
  // get the offset of RA on stack
  stack_offset = 4 * (argv[1][0] - '0');

  // call a function but illegally return
  helper();
  // the elligal return site
  DECL_LABEL(helper2_ret);
  if(grv == 0)
    exit(stack_offset/4 + 32);
  helper();
  helper();
  return grv;
}
