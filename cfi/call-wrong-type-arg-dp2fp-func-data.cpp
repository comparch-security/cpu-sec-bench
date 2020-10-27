#include "include/assembly.hpp"
#include "include/signal.hpp"

static unsigned int rv = 1;
typedef unsigned int (*func_type)(void);
static unsigned char m[] = FUNC_MACHINE_CODE;

void FORCE_NOINLINE helper(func_type fp) {
  begin_catch_nx_exception((void **)fp);
  rv = fp();
  end_catch_nx_exception();
}

int main()
{
  rv = m[0];
  helper((func_type)(&m));
  return rv;
}
