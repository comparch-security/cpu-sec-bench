#include "include/mss.hpp"
#include <set>

int main() {
  std::set<charBuffer *> pset;
  charBuffer *buffer = new charBuffer;
  pset.insert(buffer);
  delete buffer;

  for(int i=0; i<RELOC_NUM; i++) {
    buffer = new charBuffer;
    if(pset.count(buffer)) return 0;
    delete buffer;
  }

  return 1;
}
