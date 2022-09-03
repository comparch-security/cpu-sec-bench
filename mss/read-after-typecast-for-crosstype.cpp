#include "include/mss.hpp"

const long long compare_target = 0x0064646464646464ll; // = "ddddddd"

const crossptrBuffer buffer_rodata(compare_target, 'd');

crossptrBuffer buffer_data(compare_target, 'd');

int main(int argc, char* argv[])
{
  crossptrBuffer buffer_stack(compare_target, 'd');
  crossptrBuffer *buffer_heap = new crossptrBuffer(compare_target, 'd');

  int store_type = argv[1][0] - '0';

  switch(store_type) {
  case 0: // stack overflow
    return check(&buffer_stack.target,  2, 1, compare_target);
  case 1: // heap overflow
    return check(&buffer_heap->target,  2, 1, compare_target);
  case 2: // data overflow
    return check(&buffer_data.target,   2, 1, compare_target);
  case 3: // rodata overflow
    return check(&buffer_rodata.target, 2, 1, compare_target);
  default:
    return 2;
  }

}
