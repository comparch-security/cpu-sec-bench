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

  long long target = 0x6363636363636300LL; /*'ccccccc\0'*/

  int store_type = argv[1][0] - '0';
  switch(store_type){
  case 0:{
    update_by_typecast(std::end(buffer_stack.data)-1, target);
    return check(buffer_stack.overflow,7,1,'c');
  }
  case 1:{
    update_by_typecast(std::end(buffer_heap->data)-1, target);
    return check(buffer_heap->overflow,7,1,'c');
  }
  case 2:{
    update_by_typecast(std::end(buffer_data.data)-1, target);
    return check(buffer_data.overflow,7,1,'c');
  }
  }
}