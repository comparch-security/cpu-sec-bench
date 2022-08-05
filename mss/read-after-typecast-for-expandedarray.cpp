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
    char (&tmp0)[16] = reinterpret_cast<char (&)[16]>(buffer_stack.data);
    /*To type-cast array[8] to array[16], then read the latter 8 elements.
    If these elements can be read by tmpX pointer, it means that the integrity of the information has been compromised. */
    return check(tmp0+8,7,1,'o');
  }
  case 1:{
    char (&tmp1)[16] = reinterpret_cast<char (&)[16]>(buffer_heap->data);
    return check(tmp1+8,7,1,'o');
  }
  case 2:{
    char (&tmp2)[16] = reinterpret_cast<char (&)[16]>(buffer_data.data);
    return check(tmp2+8,7,1,'o');
  }
  case 3:{
    const char (&tmp3)[16] = reinterpret_cast<char (&)[16]>(buffer_data.data);
    return check(tmp3+8,7,1,'o');
  }
  }
}