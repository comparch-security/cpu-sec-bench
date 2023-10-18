#include "include/mss.hpp"
#include "include/conf.hpp"

int main(int argc, char** argv) {
  charBuffer *buffer = new charBuffer('u','d','o');
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
