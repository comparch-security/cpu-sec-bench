#include "include/assembly.hpp"
#include "include/signal.hpp"

int main()
{
  unsigned char *m = new unsigned char [16];
  assign_fake_machine_code(m);
  signal(SIGSEGV, sigsegv_handler); // catch SIGSEGV
  CALL_FUNC(m);
  signal(SIGSEGV, SIG_DFL);         // uncatch SIGSEGV
  return 0;
}
