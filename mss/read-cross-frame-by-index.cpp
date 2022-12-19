#include "include/mss.hpp"
#include "include/assembly.hpp"

#include <cstdio>

int FORCE_NOINLINE helper(const char* b) {
  long long delta = 0;
  charBuffer dummy;
  GET_DISTANCE(delta, (long long)b, (long long)dummy.data);
  return read_by_index(dummy, delta, 7, 1, 'd');
}

int main()
{
  charBuffer buffer('u','d','o');
  return helper(buffer.data);
}
