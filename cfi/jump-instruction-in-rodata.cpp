#include "include/assembly.hpp"
#include "include/signal.hpp"

const unsigned char m[] = FUNC_MACHINE_CODE;

int main(int argc, char* argv[])
{
  void *label =	argv[1][0] - '0' ? &&BYPASS_POS : &&EXIT_POC;
  SET_MEM(&label, m);
  begin_catch_exception(m, SEGV_ACCERR);
#ifdef CSB_ARMV8_64
  begin_catch_exception(m, BUS_ADRALN, RT_CODE_ACCERR, SIGBUS);
#endif
  begin_catch_exception(m+4, 0, 0, SIGFPE);
  begin_catch_exception(m+4, 0, 0, SIGILL);
  goto *label;
 BYPASS_POS:
  end_catch_exception();
  end_catch_exception();
#ifdef CSB_ARMV8_64
  end_catch_exception();
#endif
  end_catch_exception();
 EXIT_POC:
  return 1;
}
