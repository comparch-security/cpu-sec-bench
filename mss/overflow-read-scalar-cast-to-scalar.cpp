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

  switch(store_type) {
  case 0: // stack overflow
    return check(reinterpret_cast<long long*>(&buffer_stack.target),  1, 1, compare_target);
  case 1: // heap overflow
    return check(reinterpret_cast<long long*>(&buffer_heap->target),  1, 1, compare_target);
  case 2: // data overflow
    return check(reinterpret_cast<long long*>(&buffer_data.target),   1, 1, compare_target);
  case 3: // rodata overflow
    return check(reinterpret_cast<const long long*>(&buffer_rodata.target), 1, 1, compare_target);
  default:
    return 2;
  }

}