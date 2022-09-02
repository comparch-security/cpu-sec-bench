#include <unistd.h>
#include "include/mss.hpp"
#include "include/assembly.hpp"

void FORCE_NOINLINE helper(const char* b, int level) {
  long long delta = 0;
  charBuffer dummy;
  if(level != 0)
    helper(b, level-1);
  else {
    GET_DISTANCE(delta, b, dummy.data);
    update_by_pointer(dummy.data, delta, 8, 1, 'c');
  }
}

int main(int argc, char* argv[])
{
  charBuffer buffer('l', 'm', 'n');
  helper(buffer.data, sysconf(_SC_PAGESIZE)/(sizeof(charBuffer)+sizeof(long long)+sizeof(void *)) + 1);
  return check(buffer, 8, 1, 'c',1);
}
