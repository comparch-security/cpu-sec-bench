#include <cstdlib>
#include "include/assembly.hpp"

static volatile int grv = 0;

void FORCE_NOINLINE helper() {
  grv = 3;

  MOD_RET_LABEL(mid_instruction,2);

  grv = 0;
}

int main()
{
  helper();

  // failed if runs here
  grv = 1;

  MID_INSTRUCTION;

  PASS_VALUE(grv);

  exit(grv);
}
