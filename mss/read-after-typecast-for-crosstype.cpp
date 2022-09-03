#include "include/mss.hpp"

int local_check(long long num,int step,char d){
  unsigned char tmp;
  for(int i = 0; i != step; i++){
     tmp = num & 0xff;
     if(tmp != d) return 1;
     num >>= 8;
  }
  return 0;
}

const crossptrBuffer buffer_rodata(0, 'd');

crossptrBuffer buffer_data(0, 'd');

int main(int argc, char* argv[])
{
  crossptrBuffer buffer_stack(0, 'd');
  crossptrBuffer *buffer_heap = new crossptrBuffer(0, 'd');

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
