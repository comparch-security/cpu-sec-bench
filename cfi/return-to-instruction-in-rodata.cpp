#include "include/assembly.hpp"
#include "include/signal.hpp"

static unsigned int rv = 1;
const unsigned char m[] = FUNC_MACHINE_CODE;
static int helper_num=0;

void FORCE_NOINLINE helper(const unsigned char* m) {
  static int test_num=helper_num;
  ENFORCE_NON_LEAF_FUNC;
  rv = 1;
  MOD_RET_DAT(m);
}

int main()
{
  rv = m[0];
  begin_catch_nx_exception(m);
  helper(m);
  end_catch_nx_exception();
  return 0;
}

