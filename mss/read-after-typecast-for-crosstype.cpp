#include "include/mss.hpp"

void crossptr_buffer_init(crossptrBuffer *cb, long long ll, char d) {
  cb->target = ll;
  for(unsigned int i=0; i<7; i++) 
    cb->data[i] = d;
  cb->data[7] = 0;
}

int local_check(long long num,int step,char d){
  unsigned char tmp;
  for(int i = 0; i != step; i++){
     tmp = num & 0xff;
     if(tmp != d) return 1;
     num >>= 8;
  }
  return 0;
}

const crossptrBuffer buffer_rodata = {0,"ddddddd"};

crossptrBuffer buffer_data;

int main(int argc, char* argv[])
{
  // buffer in data
  crossptr_buffer_init(&buffer_data,0,'d');

  // buffer in local stack
  crossptrBuffer buffer_stack;
  crossptr_buffer_init(&buffer_stack,0,'d');

  // buffer allocated in heap
  crossptrBuffer* buffer_heap = new crossptrBuffer;
  crossptr_buffer_init(buffer_heap,0,'d');

  int store_type = argv[1][0] - '0';

  switch(store_type) {
  case 0: // stack overflow
    return local_check((*reinterpret_cast<long long (*)[2]>(&buffer_stack.target))[1],7,'d');
  case 1: // heap overflow
    return local_check((*reinterpret_cast<long long (*)[2]>(&buffer_heap->target))[1],7,'d');
  case 2: // data overflow
    return local_check((*reinterpret_cast<long long (*)[2]>(&buffer_data.target))[1],7,'d');
  case 3: // rodata overflow
    return local_check((*reinterpret_cast<const long long (*)[2]>(&buffer_rodata.target))[1],7,'d');
  default:
    return -1;
  }

}