#include "include/gcc_builtin.hpp"
#include "include/mss.hpp"

charBuffer *gbuffer;

int FORCE_NOINLINE noinline_check(const char *buf, int size, int step, char c) {
  return check(buf,size,step,c);
}

void FORCE_NOINLINE noinline_update_by_pointer(char *buf, long long offset, long long size, int step, char c) {
  update_by_pointer(buf,offset,size,step,c);
}

int FORCE_NOINLINE helper(bool option) {
  charBuffer buffer[80];
  if(option)
    return noinline_check(buffer[40].data, 8,  1, 'c');
  else
    gbuffer = buffer+40;
  return 0;
}

int main() {
  int rv0 = helper(false);
  noinline_update_by_pointer(gbuffer->data, 0, 8,  1, 'c');
  int rv1 = helper(true);
  return rv0 + rv1;
}
