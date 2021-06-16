#include <cstdlib>
#include "include/assembly.hpp"
#include "include/signal.hpp"
#include <cstdio>

static unsigned int gv = 1;
typedef unsigned int (*func_type)(void);
static unsigned char m[] = FUNC_MACHINE_CODE;

int FORCE_NOINLINE helper(func_type fp) {
  gv = fp();
  return gv;
}

int main()
{
  int rv = m[0];
  printf("dummy print: m = %p\n", m);
  begin_catch_exception(m, SEGV_ACCERR);
  begin_catch_exception(m+4, 0, 0, SIGILL);
  rv = helper((func_type)(&m));
  end_catch_exception();
  end_catch_exception();
  exit(rv);
}
