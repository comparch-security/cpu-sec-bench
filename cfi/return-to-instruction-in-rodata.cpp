#include <cstdlib>
#include "include/assembly.hpp"
#include "include/signal.hpp"

static unsigned int gv = 1;
const unsigned char m[] = FUNC_MACHINE_CODE;

int FORCE_NOINLINE helper(const unsigned char* m) {
  ENFORCE_NON_LEAF_FUNC_VAR(m[0]);
  MOD_RET_DAT(m);
  return gv;
}

int main()
{
#ifdef __x86_64
  PUSH_FAKE_RET(xlabel);
#endif
  begin_catch_nx_exception(m);
  int rv = helper(m);
  end_catch_nx_exception();
  DECL_LABEL(xlabel);
  exit(rv);
}
