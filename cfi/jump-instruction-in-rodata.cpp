#include "include/assembly.hpp"
#include "include/signal.hpp"

static unsigned int rv = 1;
const unsigned char m[] = FUNC_MACHINE_CODE;

int main()
{
  PUSH_LABEL(xlabel);
  signal(SIGSEGV, sigsegv_handler); // catch SIGSEGV
  JMP_DAT(m);
  DECL_LABEL(xlabel);
  signal(SIGSEGV, SIG_DFL);         // uncatch SIGSEGV
  rv--;
  return rv;
}
