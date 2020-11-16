#include <cstdlib>
#include "include/assembly.hpp"

static volatile int grv = 3;

void FORCE_NOINLINE helper() {
  grv = 2;

  // illegal jump target
  DECL_LABEL(helper_mid);
  grv = 0;

  // illegal jump back to main
  JMP_LABEL(main_mid, 0);
}

int main()
{
  grv = 4;
  // illegally jump to helper
  JMP_LABEL(helper_mid, 0);
  grv = 1; // failed if runs here

  // illegal jump target
  DECL_LABEL(main_mid);

  exit(grv);
}
