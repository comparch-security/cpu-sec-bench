#include <cstdint>
#include "include/assembly.hpp"

#ifdef CSB_X86_64
  #define CHECK_LEN 2
  const unsigned int  helper_pos[CHECK_LEN]  = {0, 1};
  const unsigned char helper_code[CHECK_LEN] = {0x85, 0xf6};
#endif

#ifdef CSB_RV64GC
  #define CHECK_LEN 6
  const unsigned int  helper_pos[CHECK_LEN]  = {0, 4, 5, 6, 7, 8};
  const unsigned char helper_code[CHECK_LEN] = {0x63, 0x1b, 0x86, 0xf5, 0xff, 0x02};
#endif

#ifdef CSB_ARMV8_64
  #define CHECK_LEN 6
  const unsigned int  helper_pos[CHECK_LEN]  = {0, 1, 2, 3, 4, 5};
  const unsigned char helper_code[CHECK_LEN] = {0x3f, 0x00, 0x00, 0x71, 0x4d, 0x02};
#endif

int FORCE_NOINLINE helper(unsigned char **p, int len) {
  if(len == 0) {
    // return a more stable location
    LOAD_LABEL("normal_compare", *p);
    return 1;
  }

  DECL_LABEL("normal_compare");
  for(int i=0; i<len; i++) {
    if((*p)[helper_pos[i]] != helper_code[i]) return i;
  }
  return 0;
}

int main() {
  unsigned char *fp;
  helper(&fp, 0);
  return helper(&fp, CHECK_LEN);
}
