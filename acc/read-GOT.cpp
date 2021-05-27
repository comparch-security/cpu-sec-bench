#include <cstdlib>
#include<cstdint>
#include "include/assembly.hpp"
#include "include/signal.hpp"

int stack_offset = 0;

int main(int argc, char* argv[])
{
  // get the offset of RA on stack
  stack_offset = 8 * (argv[1][0] - '0');

  void *got = NULL;
  get_got_func(&got, stack_offset);
  rand();
  mbarrier;
  return 0 != *(uintptr_t *)(got) ? 0 : 1;
}
