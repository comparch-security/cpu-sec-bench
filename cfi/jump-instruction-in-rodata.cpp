#include "include/assembly.hpp"
#include "include/signal.hpp"

static unsigned int rv = 1;
const unsigned char m[] = FUNC_MACHINE_CODE;

int main()
{
  begin_catch_exception(m, SEGV_ACCERR);
  begin_catch_exception(m+4, 0, 0, SIGILL);
  JMP_DAT(m);
  end_catch_exception();
  end_catch_exception();
  return rv;
}
