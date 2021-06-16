#include "include/assembly.hpp"
#include "include/signal.hpp"
#include <cstdio>

static unsigned int rv = 1;

unsigned char *mm;

int main()
{
  unsigned char m[64] = FUNC_MACHINE_CODE;
  mm = m;
  printf("dummy print: m = %p\n", m);
  begin_catch_exception(mm, SEGV_ACCERR);
  begin_catch_exception(mm+4, 0, 0, SIGILL);
  begin_catch_exception(mm+4, 0, 0, SIGFPE);
  JMP_DAT(mm);
  end_catch_exception();
  end_catch_exception();
  end_catch_exception();
  return rv;
}
