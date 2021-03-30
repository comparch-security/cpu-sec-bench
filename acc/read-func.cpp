#include <cstdint>
#include "include/gcc_builtin.hpp"

#define CHECK_LEN 8

const unsigned char helper_code[CHECK_LEN] =
  {
    0x31, 0xc0, 0xba, 0x31,
    0x00, 0x00, 0x00, 0x48
  };

int FORCE_NOINLINE helper(unsigned char *p) {
  for(int i=0; i<CHECK_LEN; i++) {
    if(p[i] != helper_code[i]) return 1;
  }
  return 0;
}

int main() {
  unsigned char *fp = reinterpret_cast<unsigned char *>(&helper);
  return helper(fp);
}
