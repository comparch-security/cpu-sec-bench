#include "include/assembly.hpp"

static unsigned int rv = 1;

void FORCE_NOINLINE helper(int* var) {
  rv = (*var == 0) ? 1 : 0 ;
}

int main()
{
  int var = 0;
  helper(&var);
  PASS_INT_ARG(0, helper);
  CALL_FUNC(helper);
  return rv;
}
