#include "include/assembly.hpp"
#include "include/signal.hpp"

static unsigned int rv = 1;

void FORCE_NOINLINE helper(const unsigned char* m) {
  rv = 1;
  MOD_RET_DAT(m);
  begin_catch_nx_exception(m);
}

int main()
{
  unsigned char *m = new unsigned char [16];
  assign_fake_machine_code(m);
  rv = m[0];

  PUSH_LABEL(xlabel);
  helper(m);
  DECL_LABEL(xlabel);
  end_catch_nx_exception();
  return 0;
}
