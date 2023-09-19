#include "include/mss.hpp"
#include <iostream>

const long long compare_target = 0x6464646400000001ll;

const typeCastBuffer<int> buffer_rodata(1, 'd');

typeCastBuffer<int> buffer_data(1, 'd');

int main(int argc, char* argv[])
{
  typeCastBuffer<int> buffer_stack(1, 'd');
  typeCastBuffer<int> *buffer_heap = new typeCastBuffer<int>(1, 'd');

  int store_type = argv[1][0] - '0';

  int rv;

  /* Do NOT try to optimize this case into a single expression by puting targets into a vector.
     The initialization related to target in stack and heap has been optimized away by G++ 11.2.
  */
  switch(store_type) {
  case 0: rv = check((long long *)&buffer_stack.target, 1, 1, compare_target); break;
  case 1: rv = check((long long *)&buffer_heap->target, 1, 1, compare_target); break;
  case 2: rv = check((long long *)&buffer_data.target, 1, 1, compare_target); break;
  case 3: rv = check((const long long *)&buffer_rodata.target, 1, 1, compare_target); break;
  }

  // force the initialization of buffer_heap->target
  std::cout << "dummy print to avoid dead code removal " << buffer_heap->target << std::endl;

  delete buffer_heap;
  return rv;
}
