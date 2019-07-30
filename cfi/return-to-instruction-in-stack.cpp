#include "include/assembly.hpp"
#include "include/signal.hpp"

static unsigned int rv = 1;

void FORCE_NOINLINE helper(const unsigned char* m) {
  rv = 1;
  MOD_RET_DAT(m);
}

int main()
{
  unsigned char m[] = FUNC_MACHINE_CODE;
  rv = m[0];
  begin_catch_nx_exception(m);
  helper(m);
  end_catch_nx_exception();
  return 0;
}
