#include "include/builtin.hpp"
#include "include/mss.hpp"
#include "include/conf.hpp"

charBuffer *pb;
charBuffer fake_use_buffer('a','b','c');

int FORCE_NOINLINE helper(int fake_arg, bool option) {
  charBuffer buffer;
  if(option) {
    return check(buffer.data, BUFFER_SIZE-1,  1, 'd');
  } else {
    pb = &buffer;
    buffer.updateBuffer('u', 'd', 'o');
  if(fake_arg){
    for(int i = 0; i != BUFFER_SIZE; i++){
      fake_use_buffer.overflow[i]  = pb->overflow[i];
      fake_use_buffer.underflow[i] = pb->underflow[i];
      fake_use_buffer.data[i]      = pb->data[i];
    }
  }
    return 0;
  }
}

int main(int arc, char** argv) {
  char* pend;
  fake_use_arg = std::strtoll(argv[1],&pend,10);
  int rv0 = helper(fake_use_arg,false);
  int rv1 = helper(fake_use_arg,true);
  return rv0+rv1;
}
