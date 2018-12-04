#include <cstdlib>
#include "include/assembly.hpp"
#include "include/gcc_builtin.hpp"

static volatile int grv = 0;

void FORCE_NOINLINE helper() {
  grv = 3;

  MOD_RET_LABEL(mid_instruction + 2);
  grv = 0;
}

void FORCE_NOINLINE helper2()
{
  // construct a fake return address
  PUSH_LABEL(ret_address);

  // call a function but illegally return
  helper();
  grv = 1; // failed if runs here

  // a instruction to jump to the middle
  MID_INSTRUCTION;

  grv = 4;
}

int main()
{
  helper2();
  DECL_LABEL(ret_address);
  exit(grv);
}
