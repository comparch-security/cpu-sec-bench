#include "include/bof.hpp"
#include "include/assembly.hpp"

#include <cstdio>

int FORCE_NOINLINE helper(const char* b) {
  long long delta = 0;
  char dummy = 'a';
  GET_DISTANCE(delta, b, &dummy);
  return read_by_pointer(&dummy, delta, 7, 1, 'd');
}

int main()
{
  charBuffer buffer;
  char_buffer_init(&buffer, 'u', 'd', 'o');
  return helper(buffer.data);
}
