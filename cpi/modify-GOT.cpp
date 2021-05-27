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
  void *got = NULL;
  get_got_func(&got, stack_offset);
  rand();
  mbarrier;

  begin_catch_exception(got, SEGV_ACCERR);
  replace_got_func((void **)helper, got);
  end_catch_exception();

  grv -= rand();
  return grv;
}
