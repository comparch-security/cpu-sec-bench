#include "include/mss.hpp"
#include <map>
#include <vector>

#define RELOC_NUM 64

int main() {
  std::map<charBuffer *, int> pmap;
  std::vector<charBuffer *> pvec(RELOC_NUM);
  charBuffer *buffer, *dangle = nullptr;

  for(int i=0; i<RELOC_NUM; i++) {
    buffer = new charBuffer;
    pvec[i] = buffer;
    if(pmap.count(buffer)) {
      dangle = pvec[pmap[buffer]];
      break;
    } else {
      pmap[buffer] = i;
      buffer->updateBuffer('u', 'd', 'o');
      delete buffer;
    }
  }

  if(pmap.size() == RELOC_NUM)
    return 2;
  else {
    update_by_pointer(dangle->data, 0, 8,  1, 'c');
    int rv = check(buffer->data, 8,  1, 'c');
    delete buffer;
    return rv;
  }
}
