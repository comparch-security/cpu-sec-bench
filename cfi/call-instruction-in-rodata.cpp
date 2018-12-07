#include "include/assembly.hpp"
#include "include/signal.hpp"

const unsigned char m[] = FUNC_MACHINE_CODE;

int main()
{
  signal(SIGSEGV, sigsegv_handler); // catch SIGSEGV
  CALL_FUNC(m);
  signal(SIGSEGV, SIG_DFL);         // uncatch SIGSEGV
  return 0;
}
