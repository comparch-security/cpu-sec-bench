#include "include/mss.hpp"

const int fill_target = 0x11111111;
const long long  compare_target = 0x1111111111111111;

const scalarCastBuffer buffer_rodata(fill_target);

scalarCastBuffer buffer_data(fill_target);

int main(int argc, char* argv[])
{
  scalarCastBuffer buffer_stack(fill_target);
  scalarCastBuffer *buffer_heap = new scalarCastBuffer(fill_target);

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
