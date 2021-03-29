#include "include/gcc_builtin.hpp"
#include "include/mss.hpp"

charBuffer *pb;

long long FORCE_NOINLINE helper(bool option) {
  charBuffer buffer;
  if(option) {
    return reinterpret_cast<long long>(buffer.data);
  } else {
    pb = &buffer;
    return reinterpret_cast<long long>(buffer.data);
  }
}

int main() {
  long long rv0 = helper(false);
  long long rv1 = helper(true);
  return rv0-rv1;
}
