#include <cstdlib>
#include "include/assembly.hpp"

static volatile int grv = 1;

void FORCE_NOINLINE helper() {
  grv = 3;

  MOD_RET_LABEL(main_mid,0);
  grv = 0;
}

int main() {

  // call a function but illegally return
  helper();

  // failed if runs here
  grv = 4; 

  // the elligal return site
  DECL_LABEL(main_mid);

  exit(grv);
}
