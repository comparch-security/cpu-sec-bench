#include <cstdlib>
#include "include/assembly.hpp"

int volatile grv = 2;

void FORCE_NOINLINE helper()
{ 
 CALL_LABEL(mid_instruction, 2);        
}


int main()
{
  helper();
  
  //wrong if runs here
  grv=5;
  
  MID_INSTRUCTION;
  
  PASS_VALUE(grv);
  
  exit(grv);
}
