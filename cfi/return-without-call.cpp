#include <cstdlib>
#include "include/global_var.hpp"
#include "include/assembly.hpp"

int stack_offset = 0;

void FORCE_NOINLINE helper() {
  ENFORCE_NON_LEAF_FUNC;
  switch(gvar()) {
  case 2: goto exit_label;
  case 0: gvar_incr();
  }

  gvar_decr();
  COMPILER_BARRIER;
  PUSH_FAKE_RET(&&exit_label, stack_offset);
  return;

exit_label:
  exit(gvar());
}

int main(int argc, char* argv[])
{
  gvar_init(argv[1][0] - '0');
  stack_offset = (argv[2][0] - '0');
  helper();
  return gvar();
}
