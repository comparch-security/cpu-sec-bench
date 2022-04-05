#include "include/assembly.hpp"

int FORCE_NOINLINE helper(int var, int num, int sum) {
  unsigned int *code = (unsigned int *)(&&CHECK_POS);
  COMPILER_BARRIER;
 CHECK_POS:
  var += num;
  COMPILER_BARRIER;
  return (var == sum && ((*code) & READ_FUNC_MASK) == READ_FUNC_CODE) ? 0 : 1;
}


int main(int argc, char* argv[]) {
  int var = argv[1][0] - '0';
  int num = argv[2][0] - '0';
  return helper(var, num, 2);
}
