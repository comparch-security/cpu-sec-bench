#include "include/mss.hpp"
#include "include/assembly.hpp"

charBuffer buffer_data, buffer_dataT;

int main(int argc, char* argv[])
{
  // buffer in data
  char_buffer_init(&buffer_data,  'u', 'd', 'o');
  char_buffer_init(&buffer_dataT, 'u', 'd', 'o');

  // buffer in local stack
  charBuffer buffer_stack;
  charBuffer buffer_stackT;
  char_buffer_init(&buffer_stack,  'u', 'd', 'o');
  char_buffer_init(&buffer_stackT, 'u', 'd', 'o');

  // buffer allocated in heap
  charBuffer *buffer_heap  = new charBuffer;
  charBuffer *buffer_heapT = new charBuffer;
  char_buffer_init(buffer_heap, 'u', 'd', 'o');
  char_buffer_init(buffer_heapT, 'u', 'd', 'o');

  int store_type = argv[1][0] - '0';
  long long distance = 0;

  switch(store_type) {
  case 0: { // stack
    GET_DISTANCE(distance, buffer_stackT.data, buffer_stack.data);
    if(distance > 0) {
      update_by_pointer(buffer_stack.data, 0, distance+8, 1, 'c');
      return check(buffer_stackT.data,  8,  1, 'c');
    } else {
      update_by_pointer(buffer_stackT.data, 0, -distance+8, 1, 'c');
      return check(buffer_stack.data,  8,  1, 'c');
    }
  }
  case 1: { // heap
    GET_DISTANCE(distance, buffer_heapT->data, buffer_heap->data);
    if(distance > 0) {
      update_by_pointer(buffer_heap->data, 0, distance+8, 1, 'c');
      return check(buffer_heapT->data,  8,  1, 'c');
    } else {
      update_by_pointer(buffer_heapT->data, 0, -distance+8, 1, 'c');
      return check(buffer_heap->data,  8,  1, 'c');
    }
  }
  case 2: { // data
    GET_DISTANCE(distance, buffer_dataT.data, buffer_data.data);
    if(distance > 0) {
      update_by_pointer(buffer_data.data, 0, distance+8, 1, 'c');
      return check(buffer_dataT.data,  8,  1, 'c');
    } else {
      update_by_pointer(buffer_dataT.data, 0, -distance+8, 1, 'c');
      return check(buffer_data.data,  8,  1, 'c');
    }
  }
  default:
    return -1;
  }
}
