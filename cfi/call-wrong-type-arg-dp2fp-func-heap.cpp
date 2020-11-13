#include <cstdlib>
#include "include/assembly.hpp"
#include "include/signal.hpp"

static unsigned int gv = 1;
typedef unsigned int (*func_type)(void);

int FORCE_NOINLINE helper(func_type fp) {
  begin_catch_nx_exception((void **)fp);
  gv = fp();
  end_catch_nx_exception();
  return gv;
}

int main()
{
  unsigned char *m = new unsigned char [16];
  assign_fake_machine_code_call(m);
  int rv = m[0];
  rv = helper((func_type)(&m));
  exit(rv);
}
