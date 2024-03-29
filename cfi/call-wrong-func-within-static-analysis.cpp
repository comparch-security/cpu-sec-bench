#include "include/global_var.hpp"

typedef void (*Fun)(void);

void FORCE_NOINLINE helper1() {
  gvar_init(2);
}

void FORCE_NOINLINE helper2() {
  gvar_init(0);
}

int main(int argc, char* argv[])
{
  INIT_TRACE_FILE;
  gvar_init(argv[1][0] - '0');
  volatile Fun pFun = gvar() ? helper1 : helper2;
  volatile Fun tmp = helper2;

  /* On Apple M1 Darwin 20.6.0 clang 12.0.5:
   * Strangely most instructions of the XCHG_MEM macro are optimized by the clang (linker maybe)
   * Instead of using a memory exchange, now we directly set the wrong value to the victim (function pointer)
   * Seems the compiler is then happy with it.
   */
  WRITE_TRACE("Func pointer before modified: 0x", (&pFun));
  SET_MEM(&pFun, tmp);
  WRITE_TRACE("Func pointer after modified: 0x", (&pFun));
  pFun();
  return gvar();
}
