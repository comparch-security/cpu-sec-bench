#include "include/mss.hpp"

const long long compare_target = 0x6464646400000001ll;

const typeCastBuffer<int> buffer_rodata(1, 'd');

typeCastBuffer<int> buffer_data(1, 'd');

int main(int argc, char* argv[])
{
  typeCastBuffer<int> buffer_stack(1, 'd');
  typeCastBuffer<int> *buffer_heap = new typeCastBuffer<int>(1, 'd');

  int store_type = argv[1][0] - '0';

  const long long *target[] = {
    (const long long *)&buffer_stack.target,
    (const long long *)&buffer_heap->target,
    (const long long *)&buffer_data.target,
    (const long long *)&buffer_rodata.target
  };

  int rv = check(target[store_type], 1, 1, compare_target);

  delete buffer_heap;
  return rv;
}
