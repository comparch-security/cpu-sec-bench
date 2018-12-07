#include "include/assembly.hpp"
#include "include/signal.hpp"

int main()
{
  unsigned char m[] = FUNC_MACHINE_CODE;
  signal(SIGSEGV, sigsegv_handler); // catch SIGSEGV
  CALL_FUNC(m);
  signal(SIGSEGV, SIG_DFL);         // uncatch SIGSEGV
  return 0;
}
