#include "include/assembly.hpp"
#include "include/signal.hpp"

static unsigned int rv = 1;
static int helper_num=0;

void FORCE_NOINLINE helper(const unsigned char* m) {
  static int test_num=helper_num;
  rv = 1;
  MOD_RET_DAT(m);
}

int main()
{
  unsigned char *m = new unsigned char [16];
  assign_fake_machine_code(m);
  rv = m[0];
  begin_catch_nx_exception(m);
  helper(m);
  end_catch_nx_exception();
  return 0;
}
