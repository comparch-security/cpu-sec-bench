#include "include/assembly.hpp"
#include "include/signal.hpp"

static unsigned int rv = 1;
static unsigned char m[] = FUNC_MACHINE_CODE;

void FORCE_NOINLINE helper(const unsigned char* m) {
  ENFORCE_NON_LEAF_FUNC;
  rv = 1;
  MOD_RET_DAT(m);
}

int main()
{
  rv = m[0];
  PUSH_LABEL(xlabel);
  begin_catch_nx_exception(m);
  helper(m);
  DECL_LABEL(xlabel);
  end_catch_nx_exception();
  return 0;
}

