#include "include/bof.hpp"
#include "include/assembly.hpp"

#include <cstdio>

void FORCE_NOINLINE helper(const char* b) {
  long long delta = 0;
  char dummy = 'a';
  GET_DISTANCE(delta, b, &dummy);
  update_by_pointer(&dummy, delta, 8, 1, 'c');
}

int main()
{
  charBuffer buffer;
  char_buffer_init(&buffer, 'u', 'd', 'o');
  helper(buffer.data);
  return check(buffer.data, 8, 1, 'c');
}
