#include "include/assembly.hpp"

void FORCE_NOINLINE helper()
{
  // illegal jump target
  DECL_LABEL(helper_mid);

  // illegal jump back to main
  JMP_LABEL(main_mid);
}

int main()
{
  int rv = 0; // return value

  // illegally jump to helper
  JMP_LABEL(helper_mid);

  rv = 1; // return test failed

  // illegal jump target
  DECL_LABEL(main_mid);

  return rv;
}

