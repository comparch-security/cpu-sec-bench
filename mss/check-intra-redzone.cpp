#include "include/mss.hpp"
#include "include/assembly.hpp"
#include <cstdlib>

const charBuffer buffer_rodata('u','d','o');

charBuffer buffer_data('u','d','o');

int main(int argc, char* argv[])
{
  charBuffer buffer_stack('u','d','o');

  // buffer allocated in heap
  charBuffer *buffer_heap = new charBuffer('u','d','o');

  int store_type = argv[1][0] - '0';

  long long length = 0;

  switch(store_type) {
    case 0: GET_DISTANCE(length, buffer_stack.data, buffer_stack.underflow); break;
    case 1: GET_DISTANCE(length, buffer_heap->data, buffer_heap->underflow); break;
    case 2: GET_DISTANCE(length, buffer_data.data, buffer_data.underflow); break;
    case 3: GET_DISTANCE(length, buffer_rodata.data, buffer_rodata.underflow); break;
  }
  delete buffer_heap;
  long long abs_len = llabs(length);
  if(abs_len == CB_BUF_LEN) return 0;
  else return 1;
}
