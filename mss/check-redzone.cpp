#include "include/mss.hpp"
#include "include/assembly.hpp"

long long redzone_class_var_stack,
          redzone_class_var_heap,
          redzone_class_var_data,
          redzone_class_var_rodata;

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

  GET_DISTANCE(redzone_class_var_stack,buffer_rodata.data,buffer_rodata.underflow);
  GET_DISTANCE(redzone_class_var_heap,buffer_rodata.data,buffer_rodata.underflow);
  GET_DISTANCE(redzone_class_var_data,buffer_rodata.data,buffer_rodata.underflow);
  GET_DISTANCE(redzone_class_var_rodata,buffer_rodata.data,buffer_rodata.underflow);

  if(argc == 2){//check if the platform has redzone
    if(redzone_class_var_stack != 8 || 
       redzone_class_var_heap != 8 || 
       redzone_class_var_data != 8 || 
       redzone_class_var_rodata != 8){
       return 33;//existed
    }else
       return 32;
  }else if(argc == 3){//get various redzone length
    int store_type = argv[1][0] - '0';
    int check_type = argv[2][0] - '0';
    
    if(check_type == 0) return 40;//has not redzone,return 32+8
    else if(check_type == 1){
        switch(store_type){
          case 1:
            return getPower(redzone_class_var_stack);
          case 2:
            return getPower(redzone_class_var_heap);
          case 3:
            return getPower(redzone_class_var_data);
          case 4:
            return getPower(redzone_class_var_rodata); 
        }
    }
  }
}