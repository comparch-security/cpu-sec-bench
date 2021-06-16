#include <cstdlib>
#include "include/assembly.hpp"

static volatile int grv = 2;

void FORCE_NOINLINE helper() {
  grv++;

  // illegal jump target
  DECL_LABEL(helper_mid);
  COMPILER_BARRIER;
  grv--;

  // illegal jump back to main
  JMP_LABEL(main_mid, 0);
}

int main()
{
  grv--;
  // illegally jump to helper
  JMP_LABEL(helper_mid, 0);
  grv++; // failed if runs here

  // illegal jump target
  DECL_LABEL(main_mid);

  exit(grv);
}
