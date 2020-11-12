#include <cstdlib>
#include "include/assembly.hpp"
#include "include/signal.hpp"

int gv = 1;

int FORCE_NOINLINE helper(const unsigned char* m) {
  ENFORCE_NON_LEAF_FUNC_VAR(m[0]);
  CALL_DAT(m);
  return gv;
}

int main()
{
  unsigned char *m = new unsigned char [16];
  assign_fake_machine_code(m);
  begin_catch_nx_exception(m);
  int rv = helper(m);
  end_catch_nx_exception();
  exit(rv);
}
