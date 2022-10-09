#include "include/mss.hpp"
#include "include/assembly.hpp"
#include <vector>
#include <cstdlib>

long long get_nearest_buffer(charBuffer* curr_addr, int num){
  int threshold = 4;
  long long min_gap = __LONG_LONG_MAX__, curr_gap;
  std::vector<charBuffer*> allocated_heap;
  
  for(int i = 1; i <= threshold; i++){
    charBuffer* tmp = new charBuffer[num];
    allocated_heap.push_back(tmp);
    curr_gap = llabs((unsigned long long)curr_addr - (unsigned long long)tmp);
    if(curr_gap < min_gap){
        min_gap = curr_gap;
        threshold *= 4;
    }
  }
  for(auto i = allocated_heap.begin(); i != allocated_heap.end(); i++){
    delete[] (*i);
  }
  allocated_heap.clear();
  return min_gap;
}

int main(int argc, char* argv[]){

  // buffer allocated in heap
  charBuffer *buffer_heap = new charBuffer('u','d','o');

  int store_type = argv[1][0] - '0';
  long long length = 0, abs_len;

  switch(store_type){
    case 0:
    {
      abs_len = get_nearest_buffer(buffer_heap,1);
      for(int i = 7; i >= 0; i--){
        long long aligned_len = round_up(sizeof(buffer_heap),0x1LL << i);
        if(aligned_len == abs_len){delete buffer_heap;return 0;}
      }
      break;
    }
    case 1:
    {
      GET_DISTANCE(length, buffer_heap->data, buffer_heap->underflow);
      abs_len = llabs(length);
      if(abs_len == CB_BUF_LEN){delete buffer_heap; return 0;}
      break;
    }
  }

  delete buffer_heap; // delete it to avoid trigger memory leak detection by ASan
  return -1;
}