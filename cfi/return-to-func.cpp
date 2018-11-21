#include <cstdlib>
#include "include/assembly.hpp"
#include "include/gcc_builtin.hpp"

int FORCE_NOINLINE fake_ret()
{
  exit(0);
}

void FORCE_NOINLINE helper()
{
  MOD_RET_DAT(fake_ret);
}

int main()
{
  helper();
  return 1;
}

