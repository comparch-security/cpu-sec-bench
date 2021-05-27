#include <cstdlib>
#include "include/assembly.hpp"
#include "include/signal.hpp"

static unsigned int gv = 1;
const unsigned char m[] = FUNC_MACHINE_CODE;
int stack_offset = 0;

int FORCE_NOINLINE helper(const unsigned char* m) {
  ENFORCE_NON_LEAF_FUNC_VAR(m[0]);
  MOD_STACK_DAT(m, stack_offset);
  return gv;
}

int main(int argc, char* argv[])
{
  // get the offset of RA on stack
  stack_offset = 8 * (argv[1][0] - '0');

#ifdef CSB_X86_64
  PUSH_FAKE_RET(xlabel);
#endif
  begin_catch_exception(m);
  int rv = helper(m);
  end_catch_exception();
  DECL_LABEL(xlabel);
  exit(rv);
}
