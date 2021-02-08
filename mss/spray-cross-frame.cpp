#include <cstdlib>
#include "include/mss.hpp"
#include "include/assembly.hpp"

void FORCE_NOINLINE helper(const char* b) {
  long long delta = 0;
  charBuffer dummy;
  GET_DISTANCE(delta, b, dummy.data);
  update_by_index(dummy, 0, delta+8, 1, 'c');
  exit(check(b, 8, 1, 'c'));
}

int main()
{
  charBuffer buffer;
  char_buffer_init(&buffer, 'u', 'd', 'o');
  helper(buffer.data);
  return -1;
}
