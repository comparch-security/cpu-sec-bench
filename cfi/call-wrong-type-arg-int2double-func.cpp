#include "include/assembly.hpp"

static unsigned int rv = 1;

void helper(double var) {
  rv = (var != 0.0) ? 0 : 1;
}

int main()
{
  //lp64f:  __riscv_float_abi_single 1
  //lp64d:  __riscv_float_abi_double 1
  //ilp32f: __riscv_float_abi_single 1
  //ilp32d: __riscv_float_abi_double 1
  //if the ABI is not one of these four,we don't have to test this program 
  //becasue lp32 or lp64 transfer floating points by Integer register

  num1 = argv[1][0] - '0';
  num2 = argv[2][0] - '0';
  long long tmp = -1;
  helper(2.0);
  PASS_INT_ARG(0,tmp);
  CALL_DAT(helper);
  return rv;
}
