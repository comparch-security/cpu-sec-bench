#include "include/gcc_builtin.hpp"
#include "include/mss.hpp"

charBuffer *pb;

int FORCE_NOINLINE helper(bool option) {
  charBuffer buffer;
  if(option) {
    update_by_pointer(pb->data, 0, 8,  1, 'c');
    return check(buffer.data, 7,  1, 'c');
  } else {
    pb = &buffer;
    char_buffer_init(&buffer, 'u', 'd', 'o');
    return 0;
  }
}

int main() {
  int rv0 = helper(false);
  int rv1 = helper(true);
  return rv0+rv1;
}
