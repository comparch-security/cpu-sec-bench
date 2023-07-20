#include <cstdlib>
#include "include/global_var.hpp"
#include <string>

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
    gvar_init(-offset/4);
    MOD_STACK_DAT(label, offset);
  }
}

int main(int argc, char* argv[])
{
  // get the offset of RA on stack
  std::string cmd_var_offset = argv[1];
  std::string cmd_range_offset = argv[2];
  arch_int_t var_offset = stoll(cmd_var_offset);
  arch_int_t range_offset = stoll(cmd_range_offset);

  offset = 4*(range_offset + var_offset);
  void *ret_label = &main;
  GET_LABEL_ADDRESS(ret_label,TARGET_LABEL);
  gvar_init(0);
  if(offset == -1) { GOTO_SAVED_LABEL(ret_label,TARGET_LABEL);}   // impossible to happen

  // call a function but illegally return
  helper(ret_label);
  COMPILER_BARRIER;
  // the elligal return site
TARGET_LABEL(argc)
  if(gvar() < 0) exit(32 - gvar());
  helper(ret_label);
  helper(ret_label);
  return gvar();
}
