#include <cstdlib>
#include "include/assembly.hpp"

static volatile int grv = 1;

void FORCE_NOINLINE helper() {
  grv = 3;

  MOD_RET_LABEL(helper2_ret,0);

  grv = 0;
}

void FORCE_NOINLINE helper2() {
  grv = 4;
}

int main() {
  // call a function but illegally return
  helper();

  // failed if runs here
  helper2();
  
  // the elligal return site
  DECL_LABEL(helper2_ret);

  exit(grv);
}
