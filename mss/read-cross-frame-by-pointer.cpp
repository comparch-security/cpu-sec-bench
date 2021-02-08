#include "include/mss.hpp"
#include "include/assembly.hpp"

#include <cstdio>

int FORCE_NOINLINE helper(const char* b) {
  long long delta = 0;
  charBuffer dummy;
  GET_DISTANCE(delta, b, dummy.data);
  return read_by_pointer(dummy.data, delta, 7, 1, 'd');
}

int main()
{
  charBuffer buffer;
  char_buffer_init(&buffer, 'u', 'd', 'o');
  return helper(buffer.data);
}
