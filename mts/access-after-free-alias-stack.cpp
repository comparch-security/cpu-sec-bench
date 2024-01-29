#include "include/mss.hpp"
#include "include/conf.hpp"
#include <string>
charBuffer *gbuffer;
charBuffer fake_use_buffer('a','b','c');

void FORCE_NOINLINE helper(int fake_arg) {
  charBuffer buffer_stack('u','d','o');
  gbuffer = &buffer_stack;
  if(fake_arg){
    for(int i = 0; i != BUFFER_SIZE; i++){
      fake_use_buffer.overflow[i]  = gbuffer->overflow[i];
      fake_use_buffer.underflow[i] = gbuffer->underflow[i];
      fake_use_buffer.data[i]      = gbuffer->data[i];
    }
  }
}

int main(int arc, char** argv) {
  char* pend;
  fake_use_arg = std::strtoll(argv[1],&pend,10);
  helper(fake_use_arg);
  return inline_check(gbuffer->data, BUFFER_SIZE-1,  1, 'd');
}
