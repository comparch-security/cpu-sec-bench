#include "include/assembly.hpp"
#include "include/signal.hpp"

static unsigned int rv = 1;
static unsigned char m[] = FUNC_MACHINE_CODE;

int main()
{
  PUSH_FAKE_RET(xlabel);
  begin_catch_exception(m);
  JMP_DAT(m);
  DECL_LABEL(xlabel);
  end_catch_exception();
  rv--;
  return rv;
}
