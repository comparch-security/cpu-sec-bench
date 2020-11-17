#include <cstdlib>
#include "include/assembly.hpp"

static unsigned int rv = 1;

void helper(double var) {
  rv = (var != 0.0) ? 0 : 1;
}

int main()
{
#ifndef SUPPORT_FP
  return 0; // bypass when FPU is enabled for RV64/32
#else
  long long m = -1;
  PASS_DOUBLE_ARG_FROM_INT(0, m);
  CALL_DAT(helper);
  exit(rv);
#endif
}
