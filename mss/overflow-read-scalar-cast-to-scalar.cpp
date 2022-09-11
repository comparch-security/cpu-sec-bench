#include "include/mss.hpp"

const long long compare_target = 0x6464646400000001ll;

const typeCastBuffer<int> buffer_rodata(1, 'd');

typeCastBuffer<int> buffer_data(1, 'd');

int main(int argc, char* argv[])
{
  typeCastBuffer<int> buffer_stack(1, 'd');
  typeCastBuffer<int> *buffer_heap = new typeCastBuffer<int>(1, 'd');

  int store_type = argv[1][0] - '0';

  int rv;

  switch(store_type) {
  case 0: // stack overflow
    rv = check((long long*)(&buffer_stack.target),  1, 1, compare_target); break;
  case 1: // heap overflow
    rv = check((long long*)(&buffer_heap->target),  1, 1, compare_target); break;
  case 2: // data overflow
    rv = check((long long*)(&buffer_data.target),   1, 1, compare_target); break;
  case 3: // rodata overflow
    rv = check((const long long*)(&buffer_rodata.target), 1, 1, compare_target); break;
  default:
    rv = 2;
  }

  delete buffer_heap;
  return rv;
}
