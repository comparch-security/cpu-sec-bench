#include <cstdlib>
#include "include/assembly.hpp"

void FORCE_NOINLINE helper()
{
  unsigned long long m = 0;
  PASS_INT_ARG(0, m);
  MOD_RET_DAT(exit);
}

int main()
{
  helper();
  return 3;
}
