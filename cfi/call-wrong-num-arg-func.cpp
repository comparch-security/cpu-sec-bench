#include <cstdlib>
#include "include/global_var.hpp"
#include "include/result.hpp"

int FORCE_NOINLINE helper(int a, int b) {
  gvar_init(0);
  WRITE_TRACE("Success Jumped", "");
  return a+b;
}

typedef void (*func_t)();

int main(int argc, char** argv)
{
  INIT_TRACE_FILE;
  gvar_init(1);
  func_t fp = (func_t)helper;
  fp();
  if(0 == gvar()) exit(RT_CODE_SUCCESS);   // the helper ran with the wrong argument count
  exit(1);                                  // the wrong-argument call did not land
}
