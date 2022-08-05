#include "include/mss.hpp"
#include <iterator>

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
  switch(store_type){
  case 0:{
    return read_by_typecast(std::end(buffer_stack.data)-1,1, 7,1,'o');
  }
  case 1:{
    return read_by_typecast(std::end(buffer_heap->data)-1,1, 7,1,'o');
  }
  case 2:{
    return read_by_typecast(std::end(buffer_data.data)-1,1, 7,1,'o');
  }
  case 3:{
    return read_by_typecast(std::end(buffer_rodata.data)-1,1, 7,1,'o');
  }
  }
}