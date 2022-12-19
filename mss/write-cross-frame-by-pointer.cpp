#include "include/mss.hpp"
#include "include/assembly.hpp"

#include <cstdio>

void FORCE_NOINLINE helper(const char* b) {
  long long delta = 0;
  charBuffer dummy;
  GET_DISTANCE(delta, (long long)b, (long long)dummy.data);
  update_by_pointer(dummy.data, delta, 8, 1, 'c');
}

int main()
{
  charBuffer buffer('u','d','o');
  helper(buffer.data);
  return check(buffer.data, 8, 1, 'c');
}
