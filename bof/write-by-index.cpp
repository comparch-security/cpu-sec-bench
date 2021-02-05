#include "include/bof.hpp"

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

  int  len = flow_type == 0 ?  16  : -9;

  switch(store_type*2+flow_type) {
  case 0: // stack overflow
    update_by_index(buffer_stack,  0,   16,  1, 'c');
    return check(buffer_stack.overflow,  8,  1, 'c');
  case 1: // stack underflow
    update_by_index(buffer_stack,  0,   -9, -1, 'c');
    return check(buffer_stack.underflow, 8,  1, 'c');
  case 2: // heap overflow
    update_by_index(*buffer_heap,  0,   16,  1, 'c');
    return check(buffer_heap->overflow,  8,  1, 'c');
  case 3: // heap underflow
    update_by_index(*buffer_heap,  0,   -9, -1, 'c');
    return check(buffer_heap->underflow, 8,  1, 'c');
  case 4: // data overflow
    update_by_index(buffer_data,   0,   16,  1, 'c');
    return check(buffer_data.overflow,   8,  1, 'c');
  case 5: // data underflow
    update_by_index(buffer_data,   0,   -9, -1, 'c');
    return check(buffer_data.underflow,  8,  1, 'c');
  default:
    return -1;
  }
}
