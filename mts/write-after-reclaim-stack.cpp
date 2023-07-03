#include "include/builtin.hpp"
#include "include/mss.hpp"

charBuffer *pb;

int FORCE_NOINLINE helper(bool option, char target_val, char cmp_val) {
  charBuffer buffer('u','d','o');
  if(option) {
    update_by_pointer(pb->data, 0, 8,  1, target_val);
    return check(buffer.data, 7,  1, cmp_val);
  } else {
    pb = &buffer;
    buffer.updateBuffer('u', 'd', 'o');
    return 0;
  }
}

int main(int argc, char* argv[]) {
  bool cmp_val = argv[1][0];
  bool target_val = argv[2][0];
  int rv0 = helper(false,target_val,cmp_val);
  int rv1 = helper(true,target_val,cmp_val);
  return rv0+rv1;
}
