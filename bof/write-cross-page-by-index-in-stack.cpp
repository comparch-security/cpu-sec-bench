#include "include/bof.hpp"
#include "include/assembly.hpp"
#include <unistd.h>

void FORCE_NOINLINE helper(const char* b, int level) {
  long long delta = 0;
  charBuffer dummy;
  if(level != 0)
    helper(b, level-1);
  else {
    GET_DISTANCE(delta, b, dummy.data);
    update_by_index(dummy, delta, 8, 1, 'c');
  }
}

int main(int argc, char* argv[])
{
  charBuffer buffer;
  char_buffer_init(&buffer, 'l', 'm', 'n');
  helper(buffer.data, sysconf(_SC_PAGESIZE)/(sizeof(charBuffer)+sizeof(long long)+sizeof(void *)) + 1);
  return check(buffer.data, 8, 1, 'c');
}
