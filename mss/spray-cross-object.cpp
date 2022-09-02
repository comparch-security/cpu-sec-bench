#include "include/mss.hpp"
#include "include/assembly.hpp"

// buffer in data
charBuffer buffer_data('u','d','o'), buffer_dataT('u','d','o');

int main(int argc, char* argv[])
{
  // buffer in local stack
  charBuffer buffer_stack('u','d','o');
  charBuffer buffer_stackT('u','d','o');

  // buffer allocated in heap
  charBuffer *buffer_heap  = new charBuffer('u','d','o');
  charBuffer *buffer_heapT = new charBuffer('u','d','o');

  int store_type = argv[1][0] - '0';
  long long distance = 0;

  switch(store_type) {
  case 0: { // stack
    GET_DISTANCE(distance, buffer_stackT.data, buffer_stack.data);
    if(distance > 0) {
      update_by_pointer(buffer_stack.data, 0, distance+8, 1, 'c');
      return check(buffer_stackT,  8,  1, 'c',1);
    } else {
      update_by_pointer(buffer_stackT.data, 0, -distance+8, 1, 'c');
      return check(buffer_stack,  8,  1, 'c',1);
    }
  }
  case 1: { // heap
    GET_DISTANCE(distance, buffer_heapT->data, buffer_heap->data);
    if(distance > 0) {
      update_by_pointer(buffer_heap->data, 0, distance+8, 1, 'c');
      return check(*buffer_heapT,  8,  1, 'c',1);
    } else {
      update_by_pointer(buffer_heapT->data, 0, -distance+8, 1, 'c');
      return check(*buffer_heap,  8,  1, 'c',1);
    }
  }
  case 2: { // data
    GET_DISTANCE(distance, buffer_dataT.data, buffer_data.data);
    if(distance > 0) {
      update_by_pointer(buffer_data.data, 0, distance+8, 1, 'c');
      return check(buffer_dataT,  8,  1, 'c',1);
    } else {
      update_by_pointer(buffer_dataT.data, 0, -distance+8, 1, 'c');
      return check(buffer_data,  8,  1, 'c',1);
    }
  }
  default:
    return -1;
  }
}
