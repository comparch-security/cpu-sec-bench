#include "include/assembly.hpp"
#include "include/gcc_builtin.hpp"
#include "include/signal.hpp"

static unsigned int rv = 1;

typedef unsigned int (*func_type)(void);

void FORCE_NOINLINE helper(func_type fp) {
  signal(SIGSEGV, sigsegv_handler); // catch SIGSEGV
  rv = fp();
  signal(SIGSEGV, SIG_DFL);         // uncatch SIGSEGV
}

const unsigned char m[] = FUNC_MACHINE_CODE;

int main()
{
  rv = m[0];
  helper((func_type)(&m));
  return rv;
}
