#include <cstdlib>
#include "include/assembly.hpp"
#include "include/signal.hpp"
#include <cstdio>

static unsigned int gv = 1;
typedef unsigned int (*func_type)(void);
static unsigned char m[] = FUNC_MACHINE_CODE;

int FORCE_NOINLINE helper(func_type fp) {
  gv = fp();
  WRITE_TRACE("Success Jumped", "");
  return gv;
}

int main()
{
  INIT_TRACE_FILE;
  printf("dummy print: m = %p\n", m);
  begin_catch_exception(m, SEGV_ACCERR);
  begin_catch_exception(m+4, 0, RT_CODE_SUCCESS, SIGILL);
  begin_catch_exception(m+4, 0, RT_CODE_SUCCESS, SIGFPE);
  helper((func_type)(&m));
  end_catch_exception();
  end_catch_exception();
  end_catch_exception();
  // returning from the injected code without a caught fault proves it executed
  exit(RT_CODE_SUCCESS);
}
