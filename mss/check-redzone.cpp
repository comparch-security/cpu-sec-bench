#include "include/mss.hpp"
#include "include/assembly.hpp"

long long curr_redzone_length;

const charBuffer buffer_rodata = {"uuuuuuu","ddddddd","ooooooo"};
charBuffer buffer_data;

/*We use the range [32,64) in return val to save global var,
 *But redzone's maxinum length can over 128 byte.
 *And redzone length ranges is discrete, which from the 2^1,2^2...2^7.
 *So we use the power val to reserve the global var. 
*/
inline int getPower(long long num){
  int ret = 0;
  while(num >>= 1) ret++;
  return ret+32;
}

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
  int func_type = argv[2][0] - '0';

  switch(store_type){
    case 0:
      GET_DISTANCE(curr_redzone_length,buffer_stack.data,buffer_stack.underflow);
      break;
    case 1:
      GET_DISTANCE(curr_redzone_length,buffer_heap->data,buffer_heap->underflow);
      break;
    case 2:
      GET_DISTANCE(curr_redzone_length,buffer_data.data,buffer_data.underflow);
      break;
    case 3:
      GET_DISTANCE(curr_redzone_length,buffer_rodata.data,buffer_rodata.underflow);
      break;
  }
  if(func_type){// for check test
    if(curr_redzone_length != 8) return 0;//has redzone
    else return 1;//has not redzone
  }else{//for getlen test
    return getPower(curr_redzone_length);
  }
  return -1;
}