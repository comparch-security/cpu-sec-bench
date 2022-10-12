#include "include/mss.hpp"
#include "include/assembly.hpp"
#include <vector>
#include <cstdlib>
#include <malloc.h>

int main(){

  /* Asan has hijacked "new","delete","malloc","free","malloc_usable_size"
     and other mem accessfunctions, 
     so I create an array whose length is not aligned on purpose. 
     If ASAN is not enabled, normal malloc will align it, used_ len！= unaligned_ len
     If ASAN is enabled, modified malloc will use unwritealbe region align it,
     which can be observed by malloc_usable_size().
     And used_len will equal unaligned_len.
  */
  for(int i = 1; i != 8; i++){
    size_t unaligned_len = (0x1LL << i) - 1;
    char* test_heap = new char[unaligned_len];
    size_t used_len = malloc_usable_size(test_heap);
    if(used_len != unaligned_len){
      delete[] test_heap;
      return 0;
    }
      delete[] test_heap;
  }
  return -1;
}