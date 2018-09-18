#include "include/assembly.hpp"

static volatile int grv = 0;

void FORCE_NOINLINE helper()
{
  grv = 2;

  // illegal jump target
  DECL_LABEL(helper_mid);

  grv = 0;

  // illegal jump back to main
  JMP_LABEL(main_mid);
}

int main()
{

  // illegally jump to helper
  JMP_LABEL(helper_mid);

  grv = 1;

  // illegal jump target
  DECL_LABEL(main_mid);

  return grv;
}

