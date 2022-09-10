#include "include/mss.hpp"

const long long compare_target = 0x0064646464646464ll; // = "ddddddd"

const typeCastBuffer buffer_rodata(compare_target, 'd');

typeCastBuffer buffer_data(compare_target, 'd');

int main(int argc, char* argv[])
{
  typeCastBuffer buffer_stack(compare_target, 'd');
  typeCastBuffer *buffer_heap = new typeCastBuffer(compare_target, 'd');

  int store_type = argv[1][0] - '0';

  int rv;

  switch(store_type) {
  case 0: // stack overflow
    rv = check(&buffer_stack.target,  2, 1, compare_target); break;
  case 1: // heap overflow
    rv = check(&buffer_heap->target,  2, 1, compare_target); break;
  case 2: // data overflow
    rv = check(&buffer_data.target,   2, 1, compare_target); break;
  case 3: // rodata overflow
    rv = check(&buffer_rodata.target, 2, 1, compare_target); break;
  }

  delete buffer_heap;
  return rv;
}
