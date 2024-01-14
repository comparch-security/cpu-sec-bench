#include "include/assembly.hpp"

int main(int argc, char* argv[])
{
  void *goto_label = (void*)&main;
  #ifdef DEBUG_OUTPUT
    std::cerr << "goto label is:" << goto_label << std::endl;
  #endif
  GET_LABEL_ADDRESS(goto_label,TARGET_LABEL);
  #ifdef DEBUG_OUTPUT
    std::cerr << "goto label is:" << goto_label << std::endl;
  #endif
  unsigned int instr = *(unsigned int *)(goto_label);
  if((int)(argv[1][0] - '0') == -1) { GOTO_SAVED_LABEL(goto_label);}   // impossible to happen
  COMPILER_BARRIER;
TARGET_LABEL(argc)
  #ifdef DEBUG_OUTPUT
    std::cerr << "instr is:" << std::hex << instr << std::endl;
  #endif
  // assuming only Intel x86_64 would add a endbr64 instruction here
  if((instr & 0xffffffff) == OP_ENDBR) return 32+1;
  else return 32;
}
