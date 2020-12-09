#include <stdlib.h>
#include "include/assembly.hpp"
#include "include/signal.hpp"

volatile int grv = 1;

int FORCE_NOINLINE helper() {
  return 2;
}

int main()
{
  grv = helper();
  begin_catch_nx_exception();
  replace_got_func((void **)rand, (void **)helper);
  rand();
  end_catch_nx_exception();
  grv -= rand();
  return grv;
}
