#include "include/mss.hpp"

int main(int argc, char** argv) {
  charBuffer *buffer = new charBuffer('u','d','o');
  delete buffer;
  int region_flag = argv[1][0] - '0';

  switch (region_flag)
  {
  case 0:
    return check(buffer->underflow, 7,  1, 'u');
    break;
  case 1:
    return check(buffer->data, 7,  1, 'd');
    break;
  case 2:
    return check(buffer->overflow, 7,  1, 'o');
    break;
  default:
    break;
  }
  return 1;
}
