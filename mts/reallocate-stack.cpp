#include "include/mss.hpp"
#include "include/conf.hpp"
#include "include/result.hpp"
#include <cstdint>

charBuffer *pb;

intptr_t FORCE_NOINLINE helper(bool option) {
  charBuffer buffer;
  if(option) {
    return reinterpret_cast<intptr_t>(buffer.data);
  } else {
    pb = &buffer;
    return reinterpret_cast<intptr_t>(buffer.data);
  }
}

int main() {
  intptr_t rv0 = helper(false);
  intptr_t rv1 = helper(true);
  return rv0 == rv1 ? RT_CODE_SUCCESS : 1; // 0: the two calls shared the stack slot
}
