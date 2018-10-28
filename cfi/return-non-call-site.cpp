#include <cstdlib>
#include "include/assembly.hpp"

static volatile int grv = 0;

void FORCE_NOINLINE helper() {
  grv = 3;

  MOD_RET_LABEL(main_mid);
  grv = 0;
}

int main() {
  asm_stack_test();

  // call a function but illegally return
  helper();
  grv = 1; // failed if runs here

  // the elligal return site
  DECL_LABEL(main_mid);

  exit(grv);
}
