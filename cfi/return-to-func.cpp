#include <cstdlib>
#include "include/assembly.hpp"

int FORCE_NOINLINE fake_ret() {
  exit(0);
}

void FORCE_NOINLINE helper() {
  ENFORCE_NON_LEAF_FUNC;
  MOD_RET_DAT(fake_ret);
}

int main()
{
  helper();
  return 1;
}
