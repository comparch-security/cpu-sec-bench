#include "include/assembly.hpp"

int main(int argc, char* argv[])
{
  void *goto_label = &&GOTO_SITE;
  int instr = *(int *)(goto_label);
  if(argv[1][0] - '0' == -1) goto *goto_label;   // impossible to happen
  COMPILER_BARRIER;
 GOTO_SITE:
  // assuming only Intel x86_64 would add a endbr64 instruction here
  if(instr == 0xfa1e0ff3) return 32+1;
  else return 32;
}
