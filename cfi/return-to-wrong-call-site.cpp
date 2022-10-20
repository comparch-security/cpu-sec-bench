#include <cstdlib>
#include "include/global_var.hpp"

volatile arch_int_t offset;

void FORCE_NOINLINE helper(void *label) {
  gvar_init(3);
  mbarrier;
  MOD_STACK_DAT(label, offset);
  /* HiFive Unmatched, GCC 11.2.0
   * Make sure offset is modified as otherwise
   * the stack is not expanded similarily with
   * the -within-analysis test.
   */
  offset = rand();
  gvar_init(0);
}

void FORCE_NOINLINE helper2() {
  gvar_init(4);
}

int main(int argc, char* argv[])
{
  // get the offset of RA on stack
  offset = 4 * (argv[1][0] - '0');
  void *ret_label = &&RET_POS;
  if(offset == -1) goto *ret_label;  // impossible to run here                                                                                      

  // call a function but illegally return
  helper(ret_label);
  helper2();// failed if runs here
  mbarrier;
  // the elligal return site
 RET_POS:
  exit(gvar());
}
