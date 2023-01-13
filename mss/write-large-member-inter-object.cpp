#include "include/mss.hpp"

const LargeMemberBuffer buffer_rodata('u','d','o');
// buffer in data
LargeMemberBuffer buffer_data('u','d','o');

int main(int argc, char* argv[])
{
  // buffer in local stack
  LargeMemberBuffer buffer_stack('u','d','o');

  // buffer allocated in heap
  LargeMemberBuffer *buffer_heap = new charBuffer('u','d','o');

  int store_type = argv[1][0] - '0';
  int flow_type  = argv[2][0] - '0';

  int rv;

  switch(store_type*2+flow_type) {
  case 0: // stack overflow
    update_by_pointer(buffer_stack.data, LARGE_UNALIGNED_BUF_LEN, 1,  1, 'c');
    rv = check(buffer_stack.overflow,      1,  1, 'c');
    break;
  case 1: // stack underflow
    update_by_pointer(buffer_stack.data, 0, -1, -1, 'c');
    rv = check(buffer_stack.underflow,   LARGE_UNALIGNED_BUF_LEN-1,  1, 'c');
    break;
  case 2: // heap overflow
    update_by_pointer(buffer_heap->data, LARGE_UNALIGNED_BUF_LEN, 1,  1, 'c');
    rv = check(buffer_heap->overflow,      1,  1, 'c');
    break;
  case 3: // heap underflow
    update_by_pointer(buffer_heap->data, 0, -1, -1, 'c');
    rv = check(buffer_heap->underflow,   LARGE_UNALIGNED_BUF_LEN-1,  1, 'c');
    break;
  case 4: // data overflow
    update_by_pointer(buffer_data.data,  LARGE_UNALIGNED_BUF_LEN, 1,  1, 'c');
    rv = check(buffer_data.overflow,       1,  1, 'c');
    break;
  case 5: // data underflow
    update_by_pointer(buffer_data.data,  0, -1, -1, 'c');
    rv = check(buffer_data.underflow,    LARGE_UNALIGNED_BUF_LEN-1,  1, 'c');
    break;
  default:
    rv = -1;
  }

  delete buffer_heap;
  return rv;
}
