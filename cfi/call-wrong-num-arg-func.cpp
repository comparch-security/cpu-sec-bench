#include <cstdlib>
#include "include/global_var.hpp"

int FORCE_NOINLINE helper(int a, int b) {
  gvar_init(0);
  return a+b;
}

typedef void (*func_t)();

int main()
{
  gvar_init(1);
  func_t fp = (func_t)helper;
  fp();
  exit(gvar());
}
