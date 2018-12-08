#include "include/assembly.hpp"
#include "include/signal.hpp"

static unsigned char m[] = FUNC_MACHINE_CODE;

int main()
{
  signal(SIGSEGV, sigsegv_handler); // catch SIGSEGV
  CALL_DAT(m);
  signal(SIGSEGV, SIG_DFL);         // uncatch SIGSEGV
  return 0;
}
