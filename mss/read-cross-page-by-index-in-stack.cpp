#include "include/mss.hpp"
#include "include/assembly.hpp"
#include "include/conf.hpp"

int FORCE_NOINLINE helper(const char* b, int level) {
  long long delta = 0;
  charBuffer dummy;
  if(level != 0)
    return helper(b, level-1);
  else {
    GET_DISTANCE(delta, (long long)b, (long long)dummy.data);
    return read_by_index(dummy.data, delta, BUFFER_SIZE-1, 1, 'm');
  }
}

int main(int argc, char* argv[])
{
  charBuffer buffer('l','m','n');
  return helper(buffer.data, GET_PAGE_SIZE()/(sizeof(charBuffer)+sizeof(long long)+sizeof(void *)) + 1);
}
