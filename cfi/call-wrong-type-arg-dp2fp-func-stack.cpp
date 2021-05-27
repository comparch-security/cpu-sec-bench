#include <cstdlib>
#include "include/assembly.hpp"
#include "include/signal.hpp"

static unsigned int gv = 1;
typedef unsigned int (*func_type)(void);

int FORCE_NOINLINE helper(func_type fp) {
  begin_catch_exception((void **)fp);
  gv = fp();
  end_catch_exception();
  return gv;
}

int main()
{
  unsigned char m[] = FUNC_MACHINE_CODE_CALL;
  int rv = m[0];
  rv = helper((func_type)(&m));
  exit(rv);
}
