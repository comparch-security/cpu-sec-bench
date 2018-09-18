#include "include/assembly.hpp"

void helper()
{
  // illegal jump target
  DECL_LABEL(helper_mid);

  // illegal jump back to main
  JMP_LABEL(main_mid);
}

int main()
{
  // illegally jump to helper
  JMP_LABEL(helper_mid);

  // illegal jump target
  DECL_LABEL(main_mid);

  return 0;
}

