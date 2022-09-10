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

  int rv;

  switch(store_type*2+flow_type) {
  case 0: // stack overflow
    rv = read_by_index(buffer_stack,  8, 7, 1, 'o'); break;
  case 1: // stack underflow
    rv = read_by_index(buffer_stack, -8, 7, 1, 'u'); break;
  case 2: // heap overflow
    rv = read_by_index(*buffer_heap,  8, 7, 1, 'o'); break;
  case 3: // heap underflow
    rv = read_by_index(*buffer_heap, -8, 7, 1, 'u'); break;
  case 4: // data overflow
    rv = read_by_index(buffer_data,   8, 7, 1, 'o'); break;
  case 5: // data underflow
    rv = read_by_index(buffer_data,  -8, 7, 1, 'u'); break;
  case 6: // rodata underflow
    rv = read_by_index(buffer_rodata, 8, 7, 1, 'o'); break;
  case 7: // rodata underflow
    rv = read_by_index(buffer_rodata,-8, 7, 1, 'u'); break;
  default:
    rv = -1;
  }

  delete buffer_heap;
  return rv;
}
