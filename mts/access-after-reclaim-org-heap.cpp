#include "include/mts.hpp"

int main() {
  char *buffer;

  if(!get_reallocated_block(&buffer, true))
    return 2;

  for(int i=0; i<16; i++) // except if pionter temporal protection exists
    if(buffer[i] == 'd') return 0;
  return 1;
}
