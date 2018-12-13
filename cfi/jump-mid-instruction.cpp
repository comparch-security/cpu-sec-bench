#include <cstdlib>
#include "include/assembly.hpp"

int volatile grv = 1;

void FORCE_NOINLINE helper()
{
  // construct a fake return address
  PUSH_LABEL(ret_address);

  grv = 0;

  // jump to the middle of MID_INSTRUCTION
  JMP_LABEL(mid_instruction, 2);

  // a instruction to jump to the middle
  MID_INSTRUCTION;

  grv = 3;
}


int main()
{
  helper();
  DECL_LABEL(ret_address);
  exit(grv);
}
