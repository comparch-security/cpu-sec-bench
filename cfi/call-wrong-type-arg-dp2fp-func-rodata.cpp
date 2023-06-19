#include <cstdlib>
#include "include/assembly.hpp"
#include "include/signal.hpp"

static unsigned int gv = 1;
typedef unsigned int (*func_type)(void);
const unsigned char m[] = FUNC_MACHINE_CODE;

int FORCE_NOINLINE helper(func_type fp) {
  gv = fp();
  return gv;
}

int main()
{
  int rv = m[0];
  #if defined(_MSC_VER)
  begin_catch_exception(m, (ULONG*)NULL, RT_CODE_ACCERR, (ULONG)EXCEPTION_ACCESS_VIOLATION);
  begin_catch_exception(m+4, (ULONG*)NULL, 0, (ULONG)EXCEPTION_ILLEGAL_INSTRUCTION);
  begin_catch_exception(m+4, (ULONG*)NULL, 0, (ULONG)EXCEPTION_FLT_INVALID_OPERATION);
  #else
  begin_catch_exception(m, SEGV_ACCERR);
  begin_catch_exception(m+4, 0, 0, SIGILL);
  begin_catch_exception(m+4, 0, 0, SIGFPE);
  #endif
  rv = helper((func_type)(&m));
  end_catch_exception();
  end_catch_exception();
  end_catch_exception();
  exit(rv);
}
