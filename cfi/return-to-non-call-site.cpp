#include <cstdlib>
#include "include/global_var.hpp"

volatile arch_int_t offset;

void FORCE_NOINLINE helper(void *label) {
  gvar_init(3);
  COMPILER_BARRIER;
  MOD_STACK_DAT(label, offset);
  gvar_init(0);
}

int main(int argc, char* argv[])
{
  // get the offset of RA on stack
  offset = 4 * (argv[1][0] - '0');
  void *label = &&RET_POS;
  if(offset == -1) goto *label; // impossible to go here

  gvar_init(2);
  // call a function but illegally return
  helper(label);
  gvar_init(4); // failed if runs here

  // the elligal return site
 RET_POS:
  exit(gvar());
}
