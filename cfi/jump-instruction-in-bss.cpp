#include "include/assembly.hpp"
#include "include/signal.hpp"

static unsigned int rv = 1;
static unsigned char m[] = FUNC_MACHINE_CODE;

int main()
{
  begin_catch_nx_exception(m);
  JMP_DAT(m);
  end_catch_nx_exception();
  rv--;
  return rv;
}
