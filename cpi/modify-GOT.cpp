#include <cstdlib>
#include "include/assembly.hpp"
#include "include/signal.hpp"

volatile int grv = 1;
int stack_offset = 0;

int FORCE_NOINLINE helper() {
  return 2;
}

int main(int argc, char* argv[])
{
  // get the offset of RA on stack
  stack_offset = 8 * (argv[1][0] - '0');

  grv = helper();
  begin_catch_nx_exception();
  replace_got_func((void **)helper, stack_offset);
  rand();
  end_catch_nx_exception();
  grv -= rand();
  return grv;
}
