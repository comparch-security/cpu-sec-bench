#include "include/assembly.hpp"

static unsigned int rv = 1;

void FORCE_NOINLINE helper(double var) {
  rv = (var == 0.5) ? 0 : 2;
}

int main()
{
  long long tmp = -1;
  helper(0.5);
  PASS_INT_ARG0(tmp);
  CALL_FUNC(helper);  
  return rv;
}


