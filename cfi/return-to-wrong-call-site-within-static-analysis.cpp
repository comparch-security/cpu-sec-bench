#include <cstdlib>
#include "include/assembly.hpp"

static volatile int grv = 0;

void FORCE_NOINLINE helper(int offset) {
  ENFORCE_NON_LEAF_FUNC;
  grv++;

  if(grv == 2) {
    MOD_STACK_LABEL(helper2_ret, offset);
    grv = 0;
  }
}

int main(int argc, char* argv[])
{
  // get the offset of RA on stack
  int offset = 8 * (argv[1][0] - '0');

  // call a function but illegally return
  helper(offset);
  // the elligal return site
  DECL_LABEL(helper2_ret);
  if(grv == 0)
    exit(grv);
  helper(offset);
  helper(offset);
  return grv;
}
