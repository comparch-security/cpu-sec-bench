#include <cstdlib>
#include "include/assembly.hpp"

static int helper_num=0;

void FORCE_NOINLINE helper()
{ static int test_num=helper_num;
  unsigned long long m = 0;
  PASS_INT_ARG(0, m);
  MOD_RET_DAT(exit);
}

int main()
{
  helper();
  return 3;
}
