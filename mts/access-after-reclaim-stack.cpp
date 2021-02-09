#include "include/gcc_builtin.hpp"
#include "include/mss.hpp"

int FORCE_NOINLINE helper(bool option) {
  charBuffer buffer;
  if(option)
    return check(buffer.data, 7,  1, 'd');
  else
    char_buffer_init(&buffer, 'u', 'd', 'o');
  return 0;
}

int main() {
  int rv0 = helper(false);
  int rv1 = helper(true);
  return rv0+rv1;
}
