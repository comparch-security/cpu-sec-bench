#include <cstdlib>
#include "include/global_var.hpp"

volatile arch_int_t offset;

void FORCE_NOINLINE helper(void *label) {
  gvar_incr();
  if(gvar() == 2) {
    /* On MAC M1 Clang 12.0.5
     * Due to the compiler optimization, offset is modified
     * in the MOD_STACK_DAT macro.
     * As a result, we borrow gvar to deliver the final offset
     * as used by the later exit() after EXIT_POS.
     */
    gvar_init(-offset);
    MOD_STACK_DAT(label, offset);
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
  if(gvar() < 0) exit(32 - gvar());
  helper(ret_label);
  helper(ret_label);
  return gvar();
}
