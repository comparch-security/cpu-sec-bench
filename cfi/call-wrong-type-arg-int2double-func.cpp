#include "include/assembly.hpp"

static unsigned int rv = 1;

void helper(double var) {
  rv = (var != 0.0) ? 0 : 1;
}

int main()
{
  long long m = -1;
  PASS_DOUBLE_ARG_FROM_INT(0, m);
  CALL_DAT(helper);
  return rv;
}
