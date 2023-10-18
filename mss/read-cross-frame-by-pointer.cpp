#include "include/mss.hpp"
#include "include/conf.hpp"
#include "include/assembly.hpp"

#include <cstdio>

int FORCE_NOINLINE helper(const char* b) {
  long long delta = 0;
  charBuffer dummy;
  GET_DISTANCE(delta, (long long)b, (long long)dummy.data);
  return read_by_pointer(dummy.data, delta, BUFFER_SIZE-1, 1, 'd');
}

int main()
{
  charBuffer buffer('u','d','o');
  return helper(buffer.data);
}
