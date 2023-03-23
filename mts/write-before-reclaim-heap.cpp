#include "include/mss.hpp"
#include <set>

int main() {
  std::set<charBuffer *> pset;
  charBuffer *buffer = new charBuffer('u','d','o');
  pset.insert(buffer);
  delete buffer;
  update_by_pointer(buffer->data, 0, 8,  1, 'c');

  for(int i=0; i<RELOC_NUM; i++) {
    buffer = new charBuffer;
    if(pset.count(buffer)) {
      int rv = check(buffer->data, 8,  1, 'c');
      delete buffer;
      return rv;
    } else {
      pset.insert(buffer);
      buffer->updateBuffer('u', 'd', 'o');
    }
    delete buffer;
    update_by_pointer(buffer->data, 0, 8,  1, 'c');
  }
  return 2;
}
