#include "include/assembly.hpp"

static unsigned int rv = 1;

void FORCE_NOINLINE helper(int* var) {
  rv = (*var == 0) ? 1 : 0 ;
}

int main()
{
  int var = 0;
  helper(&var);
  CALL_DAT_INT(helper, helper);
  return rv;
}
