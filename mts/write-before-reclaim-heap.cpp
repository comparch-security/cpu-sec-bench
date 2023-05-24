#include "include/mss.hpp"
#include <set>

int main(int argc, char** argv) {
  std::set<charBuffer *> pset;
  charBuffer *buffer = new charBuffer('u','d','o');
  pset.insert(buffer);
  delete buffer;
  update_by_pointer(buffer->underflow, 0, 8,  1, 't');
  update_by_pointer(buffer->data, 0, 8,  1, 'c');
  update_by_pointer(buffer->overflow, 0, 8,  1, 'n');

  int region_flag = argv[1][0] - '0';

  for(int i=0; i<RELOC_NUM; i++) {
    buffer = new charBuffer;
    if(pset.count(buffer)) {

      int rv = 1;
      switch (region_flag)
      {
      case 0:
        rv = check(buffer->underflow, 8,  1, 't');
        break;
      case 1:
        rv = check(buffer->data, 8,  1, 'c');
        break;
      case 2:
        rv = check(buffer->overflow, 8,  1, 'n');
        break;
      default:
        break;
      }     
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
