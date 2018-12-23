#include <stdlib.h>
#include "include/assembly.hpp"

volatile int grv = 1;

int FORCE_NOINLINE helper() {
  return 2;
}

int main()
{
  grv = helper();
  replace_got_func((void **)rand, (void **)helper);
  grv -= rand();
  return grv;
}

