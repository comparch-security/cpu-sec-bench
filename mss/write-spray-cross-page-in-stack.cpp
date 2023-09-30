#include <cstdlib>
#include "include/mss.hpp"
#include "include/assembly.hpp"
#include "include/conf.hpp"

void FORCE_NOINLINE helper(const char* b, int level) {
  long long delta = 0;
  charBuffer dummy;
  if(level != 0)
    helper(b, level-1);
  else {
    GET_DISTANCE(delta, (long long)b, (long long)dummy.data);
    update_by_pointer(dummy.data, 0, delta+BUFFER_SIZE, 1, 'c');
    exit(check(b, BUFFER_SIZE, 1, 'c'));
  }
}

int main(int argc, char* argv[])
{
  charBuffer buffer('l', 'm', 'n');
  helper(buffer.data, GET_PAGE_SIZE()/(sizeof(charBuffer)+sizeof(long long)+sizeof(void *)) + 1);
  return -1;
}
