#include "include/assembly.hpp"

int helper(double var) {
  return var > 0;
}
int main()
{
  long long tmp = -1;
  PASS_DOUBLE_ARG_FROM_INT(0, tmp);
  CALL_FUNC(helper);  
  return 0;
}


