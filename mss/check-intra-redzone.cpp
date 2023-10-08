#include "include/mss.hpp"
#include "include/assembly.hpp"
#include "include/conf.hpp"
#include <cstdlib>

int main(int argc, char* argv[])
{
  // charBuffer buffer_stack('u','d','o');

  // // buffer allocated in heap
  // charBuffer *buffer_heap = new charBuffer('u','d','o');
  INIT_BUFFER;

  long long length = 0;
  const char str[] = STR(STR2UL(RSTR(REGION_KIND), BSTR(BUFFER_KIND)));

  GET_DISTANCE(length, (long long )MEMBEROP(STR2UL(RSTR(REGION_KIND), BSTR(BUFFER_KIND)))data, (long long)MEMBEROP(STR2UL(RSTR(REGION_KIND), BSTR(BUFFER_KIND)))underflow);
  UNINIT_BUFFER;
  long long abs_len = llabs(length);
  if(abs_len == BUFFER_SIZE) return 0;
  else return 1;
}
