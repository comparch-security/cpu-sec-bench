#include "include/mss.hpp"
#include "include/assembly.hpp"

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
  charBuffer buffer('l','m','n');
  return helper(buffer.data, GET_PAGE_SIZE()/(sizeof(charBuffer)+sizeof(long long)+sizeof(void *)) + 1);
}
