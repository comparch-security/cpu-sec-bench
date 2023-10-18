#include "include/builtin.hpp"
#include "include/mss.hpp"
#include "include/conf.hpp"

charBuffer *pb;

int FORCE_NOINLINE helper(bool option) {
  charBuffer buffer;
  if(option) {
    return check(buffer.data, BUFFER_SIZE-1,  1, 'd');
  } else {
    pb = &buffer;
    buffer.updateBuffer('u', 'd', 'o');
    return 0;
  }
}

int main() {
  int rv0 = helper(false);
  int rv1 = helper(true);
  return rv0+rv1;
}
