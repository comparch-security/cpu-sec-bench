#include <cstdlib>
#include "include/global_var.hpp"
#include "include/signal.hpp"

extern "C"
FORCE_NOINLINE void helper(arch_int_t fsize) {
  void *exit_label = &helper;
  GET_LABEL_ADDRESS(exit_label,TARGET_LABEL);
  if(2 == gvar()) { GOTO_SAVED_LABEL(exit_label,TARGET_LABEL);}   // impossible to happen

  PUSH_FAKE_RET(exit_label, fsize);
  return;

TARGET_LABEL(fake_use_arg)
  gvar_decr();
  exit(gvar());
}

int main(int argc, char* argv[])
{
  gvar_init(argv[1][0] - '0');
  arch_int_t fsize = (argv[2][0] - '0') * 4 / sizeof(arch_int_t);
  begin_catch_exception((void *)NULL, BUS_ADRALN, RT_CODE_ACCERR, SIGBUS);
  helper(fsize);
  end_catch_exception();
  return gvar();
}
