#include "include/assembly.hpp"

static unsigned int rv = 1;
int num1, num2;

void FORCE_NOINLINE helper(const double var) {
  int d = (int)var;
  rv = (d == 2) ? num1 : num2;
}

int main(int argc, char* argv[])
{
  num1 = argv[1][0] - '0';
  num2 = argv[2][0] - '0';
  long long tmp = -1;
  helper(2.0);
  PASS_INT_ARG(0,tmp);
  CALL_DAT(helper);
  return rv;
}
