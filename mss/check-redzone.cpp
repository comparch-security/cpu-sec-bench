#include "include/mss.hpp"
#include "include/assembly.hpp"

const charBuffer buffer_rodata('u','d','o');
// buffer in data
charBuffer buffer_data('u','d','o');

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

  // buffer allocated in heap
  charBuffer *buffer_heap = new charBuffer('u','d','o');

  int store_type = argv[1][0] - '0';

  long long length;

  switch(store_type) {
  case 0: GET_DISTANCE(length, buffer_stack.data, buffer_stack.underflow); break;
  case 1: GET_DISTANCE(length, buffer_heap->data, buffer_heap->underflow); break;
  case 2: GET_DISTANCE(length, buffer_data.data, buffer_data.underflow); break;
  case 3: GET_DISTANCE(length, buffer_rodata.data, buffer_rodata.underflow); break;
  }

  delete buffer_heap; // delete it to avoid trigger memory leak detection by ASan

  /* Because each mem areas may have redzones with different lens 
  *  So it's necessary to use multiple testcases to check whether this kind mem has a redzone.
  *  And we use getdistance to determine whether it exists, 
  *  then natuarlly to combine "check" and "getXXXlen" into one test.
  */
  length -= CB_BUF_LEN;
  return 32 + getPower(length);
}
