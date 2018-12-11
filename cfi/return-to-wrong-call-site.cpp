#include <cstdlib>
#include "include/assembly.hpp"

static volatile int grv = 1;

void FORCE_NOINLINE helper() {
  grv = 3;

  MOD_RET_LABEL(helper2_ret);
  grv = 0;
}

void FORCE_NOINLINE helper2() {
  grv = 4;
}

int main() {
  asm_stack_test();
  // call a function but illegally return
  helper();
  helper2();// failed if runs here
  // the elligal return site
  DECL_LABEL(helper2_ret);

  exit(grv);
}
