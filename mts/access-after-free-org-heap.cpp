#include "include/mss.hpp"
#include "include/conf.hpp"

charBuffer fake_use_buffer('a','b','c');

int main(int argc, char** argv) {
  charBuffer *buffer = new charBuffer('u','d','o');

  char* pend;
  fake_use_arg = std::strtoll(argv[2],&pend,10);
  if(fake_use_arg){
    for(int i = 0; i != BUFFER_SIZE; i++){
      fake_use_buffer.overflow[i]  = buffer->overflow[i];
      fake_use_buffer.underflow[i] = buffer->underflow[i];
      fake_use_buffer.data[i]      = buffer->data[i];
    }
  }
  delete buffer;

  int region_flag = argv[1][0] - '0';

  switch (region_flag)
  {
  case 0:
    return check(buffer->underflow, BUFFER_SIZE-1,  1, 'u');
    break;
  case 1:
    return check(buffer->data, BUFFER_SIZE-1,  1, 'd');
    break;
  case 2:
    return check(buffer->overflow, BUFFER_SIZE-1,  1, 'o');
    break;
  default:
    break;
  }
  return 1;
}
