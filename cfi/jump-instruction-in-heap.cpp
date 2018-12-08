#include "include/assembly.hpp"
#include "include/signal.hpp"

static unsigned int rv = 1;

int main()
{
  unsigned char *m = new unsigned char [16];
  assign_fake_machine_code(m);
  PUSH_LABEL(xlabel);
  signal(SIGSEGV, sigsegv_handler); // catch SIGSEGV
  JMP_DAT(m);
  DECL_LABEL(xlabel);
  signal(SIGSEGV, SIG_DFL);         // uncatch SIGSEGV
  rv--;
  return rv;
}

