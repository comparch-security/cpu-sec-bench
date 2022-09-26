#include "include/mss.hpp"
#include "include/assembly.hpp"
#include "include/temp_file.hpp"
#include <cstdlib>
#include <fstream>

const charBuffer buffer_rodata('u','d','o');
const charBuffer buffer_rodata_dup('u','d','o');

// buffer in data
charBuffer buffer_data('u','d','o');
charBuffer buffer_data_dup('u','d','o');

/*We use the range [32,64) in return val to save global var,
 *But redzone's maxinum length can over 128 byte.
 *And redzone length ranges is discrete, which from the 2^1,2^2...2^7.
 *So we use the power val to reserve the global var. 
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
    case 0: GET_DISTANCE(length, &buffer_stack, &buffer_stack_dup);break;
    case 1: GET_DISTANCE(length, buffer_heap, buffer_heap_dup); break;
    case 2: GET_DISTANCE(length, &buffer_data, &buffer_data_dup); break;
    case 3: GET_DISTANCE(length, &buffer_rodata, &buffer_rodata_dup); break;
    case 4: GET_DISTANCE(length, buffer_stack.data, buffer_stack.underflow); break;
    case 5: GET_DISTANCE(length, buffer_heap->data, buffer_heap->underflow); break;
    case 6: GET_DISTANCE(length, buffer_data.data, buffer_data.underflow); break;
    case 7: GET_DISTANCE(length, buffer_rodata.data, buffer_rodata.underflow); break;
  }

  delete buffer_heap; // delete it to avoid trigger memory leak detection by ASan
  delete buffer_heap_dup;

  int sign = length >= 0 ? 1 : -1;
  length -= sign * (store_type < 4 ? sizeof(charBuffer) : CB_BUF_LEN);

  write_to_temp_file(length, argc, argv);
  return 64;
}
