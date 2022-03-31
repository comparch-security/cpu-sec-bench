#include <cstdlib>
#include "include/global_var.hpp"
#include "include/signal.hpp"

int FORCE_NOINLINE helper() {
  return 2;
}

int main(int argc, char* argv[])
{
  int cet_enabled = argv[1][0] - '0';
  void *rand_label = &&RAND_CALL;
  // In LLVM, goto is not allow to jump over declaration of local variables.
  void *got = NULL;

  gvar_init(helper());

  if(cet_enabled == -1) goto *rand_label;   // impossible to happen

  get_got_func(&got, rand_label, cet_enabled);
  COMPILER_BARRIER;
 RAND_CALL:
  rand();
  mbarrier;

  begin_catch_exception(got, SEGV_ACCERR);
  replace_got_func((void **)helper, got);
  end_catch_exception();

  gvar_init(gvar() - rand());
  return gvar();
}
