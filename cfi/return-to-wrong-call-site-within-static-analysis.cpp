#include <cstdlib>
#include "include/global_var.hpp"

volatile int offset;

void FORCE_NOINLINE helper(void *label) {
  gvar_incr();
  if(gvar() == 2) {
    MOD_STACK_DAT(label, offset);
    gvar_init(0);
  }
}

int main(int argc, char* argv[])
{
  // get the offset of RA on stack
  offset = 4 * (argv[1][0] - '0');
  void *ret_label = &&RET_POS;
  gvar_init(0);
  if(offset == -1) goto *ret_label;  // impossible to run here

  // call a function but illegally return
  helper(ret_label);
  COMPILER_BARRIER;
  // the elligal return site
 RET_POS:
  if(gvar() == 0) exit(offset / 4 + 32);
  helper(ret_label);
  helper(ret_label);
  return gvar();
}
