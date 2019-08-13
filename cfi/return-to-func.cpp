#include <cstdlib>
#include "include/assembly.hpp"

int FORCE_NOINLINE fake_ret() {
  exit(0);
}

void FORCE_NOINLINE helper() {
  MOD_RET_DAT(fake_ret);
}

int main()
{
  asm_stack_test();
  helper();
  return 1;
}
