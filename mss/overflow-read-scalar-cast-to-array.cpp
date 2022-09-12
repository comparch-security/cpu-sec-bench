#include "include/mss.hpp"

const long long compare_target = 0x0064646464646464ll; // = "ddddddd"

const typeCastBuffer<long long> buffer_rodata(compare_target, 'd');

typeCastBuffer<long long> buffer_data(compare_target, 'd');

int main(int argc, char* argv[])
{
  typeCastBuffer<long long> buffer_stack(compare_target, 'd');
  typeCastBuffer<long long> *buffer_heap = new typeCastBuffer<long long>(compare_target, 'd');

  int store_type = argv[1][0] - '0';

  int rv;

  switch(store_type) {
  case 0: rv = check((long long *)&buffer_stack.target, 2, 1, compare_target); break;
  case 1: rv = check((long long *)&buffer_heap->target, 2, 1, compare_target); break;
  case 2: rv = check((long long *)&buffer_data.target, 2, 1, compare_target); break;
  case 3: rv = check((const long long *)&buffer_rodata.target, 2, 1, compare_target); break;
  }

  delete buffer_heap;
  return rv;
}
