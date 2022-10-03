#include "include/mss.hpp"
#include "include/assembly.hpp"
#include <vector>
#include <cstdlib>

#ifdef X86_64
  /* X86_64 ABI demands array which is longer then 16 bytes
  * should be aligned to 16 bytes.
  * And For gcc asan
  * array which is longer then 16 bytes is aligned to 8 bytes.
  * Finally it is aligned to 16 bytes.
  */
  #define ALLOCBUFFER_ALIGN_GRANULARITY 16
#elif AARCH64
  /* Cite from aapcs64 ABI 
   * 'The alignment of an array shall be the alignment of its base type',
   * So charBuffer array is aligned to 1 byte.
   * And For gcc asan
   * array which is longer then 16 bytes is aligned to 8 bytes.
   * Finally it is aligned to 8 bytes.
   */
  #define ALLOCBUFFER_ALIGN_GRANULARITY 8
#else
  #define ALLOCBUFFER_ALIGN_GRANULARITY 16
#endif

std::vector<charBuffer*> allocated_heap;

inline charBuffer *get_nearest_buffer(charBuffer* curr_addr){
  int threshold = 4;
  long long min_gap = __LONG_LONG_MAX__;
  charBuffer* target = NULL;
  
  for(int i = 1; i <= threshold; i++){
    charBuffer* tmp = new charBuffer('u','d','o');
    allocated_heap.push_back(tmp);
    long long curr_gap = llabs((unsigned long long)curr_addr - (unsigned long long)tmp);
    if(curr_gap < min_gap){
        target = tmp;
        min_gap = curr_gap;
        threshold = 4*i;
    }
  }
  return target;
}

int main(int argc, char* argv[]){

  // buffer allocated in heap
  charBuffer *buffer_heap = new charBuffer('u','d','o');
  charBuffer *buffer_heap_dup = get_nearest_buffer(buffer_heap);

  int store_type = argv[1][0] - '0';
  long long length = 0;

  switch(store_type){
    case 0: GET_DISTANCE(length, buffer_heap, buffer_heap_dup); break;
    case 1: GET_DISTANCE(length, buffer_heap->data, buffer_heap->underflow); break;
  }

  long long aligned_len = store_type == 0 ? 
  round_up(sizeof(charBuffer),ALLOCBUFFER_ALIGN_GRANULARITY) :
  round_up(CB_BUF_LEN,1);

  delete buffer_heap; // delete it to avoid trigger memory leak detection by ASan
  for(auto i = allocated_heap.begin(); i != allocated_heap.end(); i++){
    delete *i;
  }
  allocated_heap.clear();

  if(aligned_len != llabs(length)){
    /* current mem type has redzone */
    return -1;
  }else
    return 0;
}