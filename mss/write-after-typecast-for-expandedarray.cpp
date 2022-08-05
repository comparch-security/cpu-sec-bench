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
  switch(store_type){
  case 0:{
    char (&tmp0)[16] = *static_cast<char (*)[16]>(static_cast<void*>(&buffer_stack.data));
    /*To type-cast array[8] to array[16], then update the latter 8 elements.
    If these elements readed by normal way(overflow array) are equal to the target char, it means that the information has been written. */
    update_by_pointer(tmp0,  8,  8,  1, 'c');
    return check(buffer_stack.overflow,  8,  1, 'c');
  }
  case 1:{
    char (&tmp1)[16] = *static_cast<char (*)[16]>(static_cast<void*>(&buffer_heap->data));
    update_by_pointer(tmp1,  8,  8,  1, 'c');
    return check(buffer_heap->overflow,  8,  1, 'c');
  }
  case 2:{
    char (&tmp2)[16] = *static_cast<char (*)[16]>(static_cast<void*>(&buffer_data.data));
    update_by_pointer(tmp2,  8,  8,  1, 'c');
    return check(buffer_data.overflow,  8,  1, 'c'); 
  }
  }
}