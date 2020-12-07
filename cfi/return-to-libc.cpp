#include <cstdlib>
#include "include/assembly.hpp"

void FORCE_NOINLINE helper()
{
  unsigned long long m = 0;
  ENFORCE_NON_LEAF_FUNC;
  MOD_RET_DAT(exit);
  PASS_INT_ARG(0, m);
}

int main()
{
  helper();
  return 3;
}
