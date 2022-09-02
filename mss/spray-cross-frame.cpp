#include <cstdlib>
#include "include/mss.hpp"
#include "include/assembly.hpp"

void FORCE_NOINLINE helper(const charBuffer& b) {
  long long delta = 0;
  charBuffer dummy;
  GET_DISTANCE(delta, b.data, dummy.data);
  update_by_index(dummy, 0, delta+8, 1, 'c');
  exit(check(b, 8, 1, 'c',1));
}

int main()
{
  charBuffer buffer('u','d','o');
  helper(buffer);
  return -1;
}
