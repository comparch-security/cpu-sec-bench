#include <cstdint>
#include "include/assembly.hpp"

#ifdef CSB_X86_64
  #define CHECK_LEN 2
  const unsigned int  helper_pos[CHECK_LEN]  = {0,    1};
  const unsigned char helper_code[CHECK_LEN] = {0x85, 0xf6};
#endif

#ifdef CSB_RV64GC
  #define CHECK_LEN 3
  const unsigned int  helper_pos[CHECK_LEN]  = {0,    4,    5};
  const unsigned char helper_code[CHECK_LEN] = {0x63, 0x1c, 0x61};
#endif

#ifdef CSB_ARMV8_64
  #define CHECK_LEN 3
  const unsigned int  helper_pos[CHECK_LEN]  = {0,    2,    3};
  const unsigned char helper_code[CHECK_LEN] = {0x3f, 0x00, 0x71};
#endif

int FORCE_NOINLINE helper(unsigned char **p, int len) {
  if(len == 0) {
    // return a more stable location
    LOAD_LABEL("normal_compare", *p);
    return 1;
  }

  DECL_LABEL("normal_compare");
  for(int i=0; i<len; i++) {
    if((*p)[helper_pos[i]] != helper_code[i]) return 2+i;
  }
  return 0;
}

int main() {
  unsigned char *fp;
  helper(&fp, 0);
  return helper(&fp, CHECK_LEN);
}
