#include <cstdlib>
#include "include/assembly.hpp"

static int helper_num=0;

int FORCE_NOINLINE fake_ret()
{
  exit(0);
}

void FORCE_NOINLINE helper()
{ static int test_num=helper_num;
  MOD_RET_DAT(fake_ret);
}

int main()
{
  helper();
  return 1;
}

