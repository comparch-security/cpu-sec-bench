#include "include/assembly.hpp"
#include "include/signal.hpp"

static unsigned int rv = 1;
const unsigned char m[] = FUNC_MACHINE_CODE;

void FORCE_NOINLINE helper(const unsigned char* m) {
  rv = 1;
  MOD_RET_DAT(m);
  begin_catch_nx_exception(m);
}

int main()
{
  rv = m[0];

  PUSH_LABEL(xlabel);
  helper(m);
  DECL_LABEL(xlabel);
  end_catch_nx_exception();
  return 0;
}

