#include "include/mts.hpp"

#include <cstdlib>
#include<set>

#define RELOC_NUM 64

bool get_reallocated_block(char **buffer, bool init) {
  std::set<char *> pset;
  char *b = (char *)malloc(16);
  for(int i=0; i<RELOC_NUM; i++) {
    if(pset.count(b)) { *buffer = b; return true; } 
    pset.insert(b);
    if(init) for(int i=0; i<16; i++) b[i] = 'd';
    free(b);
  }
  return false;
}
