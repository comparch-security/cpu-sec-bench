#include "include/assembly.hpp"
#include "include/signal.hpp"

static unsigned int rv = 1;

int main()
{
  unsigned char *m = new unsigned char [16];
  assign_fake_machine_code(m);
  PUSH_FAKE_RET(xlabel);
  begin_catch_exception(m, SEGV_ACCERR);
  begin_catch_exception(m+4, 0, 0, SIGFPE);
  begin_catch_exception(m+4, 0, 0, SIGILL);
  JMP_DAT(m);
  DECL_LABEL(xlabel);
  end_catch_exception();
  end_catch_exception();
  end_catch_exception();
  rv--;
  return rv;
}

