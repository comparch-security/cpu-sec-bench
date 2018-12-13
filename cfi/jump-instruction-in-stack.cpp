#include "include/assembly.hpp"
#include "include/signal.hpp"

static unsigned int rv = 1;

int main()
{
  unsigned char m[] = FUNC_MACHINE_CODE;
  PUSH_LABEL(xlabel);
  begin_catch_nx_exception(m);
  JMP_DAT(m);
  DECL_LABEL(xlabel);
  end_catch_nx_exception();
  rv--;
  return rv;
}
