#include "include/mss.hpp"
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
  return rv0-rv1;
}
