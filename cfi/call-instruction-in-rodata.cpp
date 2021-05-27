#include <cstdlib>
#include "include/assembly.hpp"
#include "include/signal.hpp"

int gv = 1;
const unsigned char m[] = FUNC_MACHINE_CODE_CALL;

int FORCE_NOINLINE helper(const unsigned char* m) {
  ENFORCE_NON_LEAF_FUNC_VAR(m[0]);
  CALL_DAT(m);
  return gv;
}

int main()
{
  begin_catch_exception(m);
  int rv = helper(m);
  end_catch_exception();
  exit(rv);
}
