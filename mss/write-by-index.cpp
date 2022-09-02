#include "include/mss.hpp"

const charBuffer buffer_rodata('u','d','o');
// buffer in data
charBuffer buffer_data('u','d','o');

int main(int argc, char* argv[])
{

  // buffer in local stack
  charBuffer buffer_stack('u','d','o');

  // buffer allocated in heap
  charBuffer *buffer_heap = new charBuffer('u','d','o');

  int store_type = argv[1][0] - '0';
  int flow_type  = argv[2][0] - '0';

  switch(store_type*2+flow_type) {
  case 0: // stack overflow
    update_by_index(buffer_stack,  0,   16,  1, 'c');
    return check(buffer_stack,  8,  1, 'c',2);
  case 1: // stack underflow
    update_by_index(buffer_stack,  0,   -9, -1, 'c');
    return check(buffer_stack, 8,  1, 'c',0);
  case 2: // heap overflow
    update_by_index(*buffer_heap,  0,   16,  1, 'c');
    return check(*buffer_heap,  8,  1, 'c',2);
  case 3: // heap underflow
    update_by_index(*buffer_heap,  0,   -9, -1, 'c');
    return check(*buffer_heap, 8,  1, 'c',0);
  case 4: // data overflow
    update_by_index(buffer_data,   0,   16,  1, 'c');
    return check(buffer_data,   8,  1, 'c',2);
  case 5: // data underflow
    update_by_index(buffer_data,   0,   -9, -1, 'c');
    return check(buffer_data,  8,  1, 'c',0);
  default:
    return -1;
  }
}
