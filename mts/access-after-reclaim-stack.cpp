#include "include/mss.hpp"

#if defined(__GNUC__) 
  #include "include/posix/gcc_builtin.hpp"
#else
  #include "include/visualc++/msvc_builtin.hpp"
#endif

charBuffer *pb;

int FORCE_NOINLINE helper(bool option) {
  charBuffer buffer;
  if(option) {
    return check(buffer.data, 7,  1, 'd');
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
