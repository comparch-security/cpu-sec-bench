#include <cstdlib>
#include "include/global_var.hpp"

void FORCE_NOINLINE helper(double var) {
  gvar_init((var != 0.0) ? 0 : 2);
}

static double d;

int main()
{
  gvar_init(1);
#ifndef SUPPORT_FP
  return 0; // bypass when FPU is enabled for RV64/32
#else
  arch_int_t *i = (arch_int_t *)(&d);
  *i = -1;  // use the memory as way to pass an int to the double arguemnt
  mbarrier;
  helper(d);
  exit(gvar());
#endif
}
