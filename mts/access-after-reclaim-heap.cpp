#include "include/mss.hpp"
#include <set>

#define RELOC_NUM 64

int main() {
  std::set<charBuffer *> pset;
  charBuffer *buffer = new charBuffer;
  char_buffer_init(buffer, 'u', 'd', 'o');
  pset.insert(buffer);
  delete buffer;

  for(int i=0; i<RELOC_NUM; i++) {
    buffer = new charBuffer;
    if(pset.count(buffer))
      return check(buffer->data, 7,  1, 'd');
    delete buffer;
  }
  return 2;
}
