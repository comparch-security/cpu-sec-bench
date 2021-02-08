#include "include/mss.hpp"

const charBuffer buffer_rodata = {"uuuuuuu","ddddddd","ooooooo"};
charBuffer buffer_data;

int main(int argc, char* argv[])
{
  // buffer in data
  char_buffer_init(&buffer_data, 'u', 'd', 'o');

  // buffer in local stack
  charBuffer buffer_stack;
  char_buffer_init(&buffer_stack, 'u', 'd', 'o');

  // buffer allocated in heap
  charBuffer *buffer_heap = new charBuffer;
  char_buffer_init(buffer_heap, 'u', 'd', 'o');

  int store_type = argv[1][0] - '0';
  int flow_type  = argv[2][0] - '0';

  switch(store_type*2+flow_type) {
  case 0: // stack overflow
    return read_by_index(buffer_stack,  8, 7, 1, 'o');
  case 1: // stack underflow
    return read_by_index(buffer_stack, -8, 7, 1, 'u');
  case 2: // heap overflow
    return read_by_index(*buffer_heap,  8, 7, 1, 'o');
  case 3: // heap underflow
    return read_by_index(*buffer_heap, -8, 7, 1, 'u');
  case 4: // data overflow
    return read_by_index(buffer_data,   8, 7, 1, 'o');
  case 5: // data underflow
    return read_by_index(buffer_data,  -8, 7, 1, 'u');
  case 6: // rodata underflow
    return read_by_index(buffer_rodata, 8, 7, 1, 'o');
  case 7: // rodata underflow
    return read_by_index(buffer_rodata,-8, 7, 1, 'u');
  default:
    return -1;
  }
}
