#include <cstdlib>
#include "include/assembly.hpp"

static volatile int grv = 1;

int FORCE_NOINLINE helper(int a, int b) {
  grv = 0;
  return a+b;
}

int main()
{
  CALL_DAT(helper);
  exit(grv);
}
