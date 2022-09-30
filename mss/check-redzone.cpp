#include "include/mss.hpp"
#include "include/assembly.hpp"
#include "include/temp_file.hpp"
#include <cstdlib>

const charBuffer buffer_rodata('u','d','o');
const charBuffer buffer_rodata_dup('u','d','o');

// buffer in data
charBuffer buffer_data('u','d','o');
charBuffer buffer_data_dup('u','d','o');

void get_heapmem_rzlen(long long &length, size_t var_size){
  long long rounded_size;
  if(var_size <= 16)
    rounded_size = round_up(var_size,16);
  else
    rounded_size = round_up(var_size,8);
  length -= (length > 0 ? rounded_size : -rounded_size);
}

void get_othermem_rzlen(long long &length, size_t var_size){

  long long rounded_size;
  rounded_size = round_up(var_size,32);
  length -= (length > 0 ? rounded_size : -rounded_size);
}

/*
 *Redzone length ranges is discrete, which from the 2^1,2^2...2^7.
*/
inline int getPower(long long num){
  int ret = 0;
  while(num >>= 1) ret++;
  return ret;
}

int main(int argc, char* argv[])
{

  // buffer in local stack
  charBuffer buffer_stack('u','d','o');
  charBuffer buffer_stack_dup('u','d','o');

  // buffer allocated in heap
  charBuffer *buffer_heap = new charBuffer('u','d','o');
  charBuffer *buffer_heap_dup = new charBuffer('u','d','o');

  int store_type = argv[1][0] - '0';

  long long length = 0;

  switch(store_type) {
    case 0: GET_DISTANCE(length, &buffer_stack, &buffer_stack_dup);
            get_othermem_rzlen(length, sizeof(charBuffer));break;
    case 1: GET_DISTANCE(length, buffer_heap, buffer_heap_dup);
            get_heapmem_rzlen(length, sizeof(charBuffer));break;
    case 2: GET_DISTANCE(length, &buffer_data, &buffer_data_dup);
            get_othermem_rzlen(length, sizeof(charBuffer));break;
    case 3: GET_DISTANCE(length, &buffer_rodata, &buffer_rodata_dup);
            get_othermem_rzlen(length, sizeof(charBuffer));break;
    case 4: GET_DISTANCE(length, buffer_stack.data, buffer_stack.underflow); break;
    case 5: GET_DISTANCE(length, buffer_heap->data, buffer_heap->underflow); break;
    case 6: GET_DISTANCE(length, buffer_data.data, buffer_data.underflow); break;
    case 7: GET_DISTANCE(length, buffer_rodata.data, buffer_rodata.underflow); break;
  }

  delete buffer_heap; // delete it to avoid trigger memory leak detection by ASan
  delete buffer_heap_dup;

  if(is_power_of_two(llabs(length))){
     write_to_temp_file(length, argc, argv);
     return 64;
  }
  else return -1;
}
