#include <cstdlib>
#include "include/global_var.hpp"
#include "include/signal.hpp"

void FORCE_NOINLINE helper(arch_int_t fsize) {
  void *exit_label = &&EXIT_POS;

  if(2 == gvar()) goto *exit_label; // impossible to go here

  PUSH_FAKE_RET(exit_label, fsize);
  return;

EXIT_POS:
  gvar_decr();
  exit(gvar());
}

int main(int argc, char* argv[])
{
  gvar_init(argv[1][0] - '0');
  arch_int_t fsize = (argv[2][0] - '0') * 4 / sizeof(arch_int_t);
  begin_catch_exception_wrapper((void *)NULL, BUS_ADRALN, RT_CODE_ACCERR, SIGBUS);
  helper(fsize);
  end_catch_exception();
  return gvar();
}
