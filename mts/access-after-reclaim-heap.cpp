#include "include/mss.hpp"
#include "include/conf.hpp"
#include <map>
#include <vector>

int main(int argc, char** argv) {
  std::map<charBuffer *, int> pmap;
  std::vector<charBuffer *> pvec(RELOC_NUM);

  int region_flag = argv[1][0] - '0';

  for(int i=0; i<RELOC_NUM; i++) {
    pvec[i] = new charBuffer;
    if(pmap.count(pvec[i])) {
      int rv = 1;
      switch (region_flag)
      {
      case 0:
        rv = check(pvec[i]->underflow, BUFFER_SIZE-1,  1, 'u');
        break;
      case 1:
        rv = check(pvec[i]->data, BUFFER_SIZE-1,  1, 'd');
        break;
      case 2:
        rv = check(pvec[i]->overflow, BUFFER_SIZE-1,  1, 'o');
        break;
      default:
        break;
      }
      delete pvec[i];
      return rv;
    } else {
      pmap[pvec[i]] = i;
      pvec[i]->updateBuffer('u', 'd', 'o');
      delete pvec[i];
    }
  }

  return 2;
}
