#include "include/mss.hpp"

const long long compare_target = 0x0064646464646464ll; // = "ddddddd"

const typeCastBuffer<long long> buffer_rodata(compare_target, 'd');

typeCastBuffer<long long> buffer_data(compare_target, 'd');

int main(int argc, char* argv[])
{
  typeCastBuffer<long long> buffer_stack(compare_target, 'd');
  typeCastBuffer<long long> *buffer_heap = new typeCastBuffer<long long>(compare_target, 'd');

  int store_type = argv[1][0] - '0';

  const long long *target[] = {&buffer_stack.target, &buffer_heap->target, &buffer_data.target, &buffer_rodata.target};

  int rv = check(target[store_type],  2, 1, compare_target);

  delete buffer_heap;
  return rv;
}
