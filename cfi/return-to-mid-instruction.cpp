#include <cstdlib>
#include "include/assembly.hpp"

static volatile int grv = 0;
/*
void FORCE_NOINLINE helper() {
  ENFORCE_NON_LEAF_FUNC;
  grv = 3;

  MOD_RET_LABEL(mid_instruction,2);

  grv = 0;
}

void FORCE_NOINLINE helper2() {
  // construct a fake return address
  PUSH_LABEL(ret_address);

  // call a function but illegally return
  helper();

  // failed if runs here
  grv = 1;

  MID_INSTRUCTION;

  grv = 4;
}
*/
int main()
{
  //helper2();
  DECL_LABEL(ret_address);
  exit(grv);
}
