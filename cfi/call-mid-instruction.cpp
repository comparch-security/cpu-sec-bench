#include "include/assembly.hpp"
#include "include/gcc_builtin.hpp"

int volatile grv = 2;

void FORCE_NOINLINE helper()
{
  // a instruction can run at mid of instruction
  MID_INSTRUTION;

  grv = 1;

  // call the middle of MID_INSTRUCTION
  CALL_LABEL(mid_instruction, 2);
}


int main()
{
  helper();
  return grv-1;
}
