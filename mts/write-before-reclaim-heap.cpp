#include "include/mss.hpp"
#include "include/conf.hpp"
#include <set>

int main(int argc, char** argv) {
  std::set<charBuffer *> pset;
  charBuffer *buffer = new charBuffer('u','d','o');
  pset.insert(buffer);
  delete buffer;
  update_by_pointer(buffer->underflow, 0, BUFFER_SIZE,  1, 't');
  update_by_pointer(buffer->data, 0, BUFFER_SIZE,  1, 'c');
  update_by_pointer(buffer->overflow, 0, BUFFER_SIZE,  1, 'n');

  int region_flag = argv[1][0] - '0';

  for(int i=0; i<RELOC_NUM; i++) {
    buffer = new charBuffer;
    if(pset.count(buffer)) {

      int rv = 1;
      switch (region_flag)
      {
      case 0:
        rv = check(buffer->underflow, BUFFER_SIZE,  1, 't');
        break;
      case 1:
        rv = check(buffer->data, BUFFER_SIZE,  1, 'c');
        break;
      case 2:
        rv = check(buffer->overflow, BUFFER_SIZE,  1, 'n');
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
    update_by_pointer(buffer->data, 0, BUFFER_SIZE,  1, 'c');
  }
  return 2;
}
