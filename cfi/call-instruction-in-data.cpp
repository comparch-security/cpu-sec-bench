#include <cstdlib>
#include "include/assembly.hpp"
#include "include/signal.hpp"
#include <cstdio>

int gv = 1;
static unsigned char m[] = FUNC_MACHINE_CODE;

int FORCE_NOINLINE helper(const unsigned char* m) {
  ENFORCE_NON_LEAF_FUNC_VAR(m[0]);
  CALL_DAT(m);
  return gv;
}

int main()
{
  printf("dummy print: m = %p\n", m);
  begin_catch_exception(m, SEGV_ACCERR);
#ifdef CSB_ARMV8_64
  // bus error on Apple M1
  begin_catch_exception(m, BUS_ADRALN, RT_CODE_ACCERR, SIGBUS); // no sure why it is address alignment error
#endif
  begin_catch_exception(m+4, 0, 0, SIGFPE);
  begin_catch_exception(m+4, 0, 0, SIGILL);
  int rv = helper(m);
  end_catch_exception();
#ifdef CSB_ARMV8_64
  end_catch_exception();
#endif
  end_catch_exception();
  end_catch_exception();
  exit(rv);
}
