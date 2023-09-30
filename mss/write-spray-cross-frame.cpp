#include <cstdlib>
#include "include/mss.hpp"
#include "include/conf.hpp"
#include "include/assembly.hpp"

void FORCE_NOINLINE helper(const char* b) {
  long long delta = 0;
  charBuffer dummy;
  GET_DISTANCE(delta, (long long)b, (long long)dummy.data);
  update_by_index(dummy.data, 0, delta+BUFFER_SIZE, 1, 'c');
  exit(check(b, BUFFER_SIZE, 1, 'c'));
}

int main()
{
  charBuffer buffer('u','d','o');
  helper(buffer.data);
  return -1;
}
