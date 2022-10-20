#include <cstdlib>
#include "include/global_var.hpp"

volatile arch_int_t offset;

void FORCE_NOINLINE helper(void *label) {
  gvar_init(0);
  MOD_STACK_DAT(label, offset);
  /* HiFive Unmatched, GCC 11.2.0
   * Make sure offset is modified as otherwise
   * the stack is not expanded similarily with
   * the -within-analysis test.
   */
  offset = rand();
  gvar_init(3);
  mbarrier;
  PASS_INT_ARG0_IMM(0);
}

int main(int argc, char* argv[])
{
  // get the offset of RA on stack
  offset = 4 * (argv[1][0] - '0');

  void *label = (void *)exit;
  helper(label);
  return gvar();
}
