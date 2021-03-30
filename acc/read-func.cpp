#include <cstdint>
#include "include/gcc_builtin.hpp"

#define CHECK_LEN 6

const unsigned int  helper_pos[CHECK_LEN] =
  {
    0, 1, 2, 4, 5, 7
  };
const unsigned char helper_code[CHECK_LEN] =
  {
    0x85, 0xf6, 0x7e,
    0x80, 0x3f, 0x75,
  };

int FORCE_NOINLINE helper(unsigned char *p, int len) {
  for(int i=0; i<len; i++) {
    if(p[helper_pos[i]] != helper_code[i]) return 1;
  }
  return 0;
}

int main() {
  unsigned char *fp = reinterpret_cast<unsigned char *>(&helper);
  return helper(fp, CHECK_LEN);
}
