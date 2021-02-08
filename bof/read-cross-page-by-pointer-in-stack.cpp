#include "include/bof.hpp"
#include "include/assembly.hpp"
#include <unistd.h>

int FORCE_NOINLINE helper(const char* b, int level) {
  long long delta = 0;
  charBuffer dummy;
  if(level != 0)
    return helper(b, level-1);
  else {
    GET_DISTANCE(delta, b, dummy.data);
    return read_by_pointer(dummy.data, delta, 7, 1, 'm');
  }
}

int main(int argc, char* argv[])
{
  charBuffer buffer;
  char_buffer_init(&buffer, 'l', 'm', 'n');
  return helper(buffer.data, sysconf(_SC_PAGESIZE)/(sizeof(charBuffer)+sizeof(long long)+sizeof(void *)) + 1);
}
