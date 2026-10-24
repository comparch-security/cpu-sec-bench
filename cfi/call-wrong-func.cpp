#include "include/global_var.hpp"
#include "include/result.hpp"

typedef void (*Fun)(void);

void FORCE_NOINLINE helper1() {
  gvar_init(2);
}

void FORCE_NOINLINE helper2() {
  gvar_init(3);
}

void FORCE_NOINLINE helper3() {
  gvar_init(0);
  WRITE_TRACE("Success Jumped", "");
}

int main(int argc, char* argv[])
{
  INIT_TRACE_FILE;
  volatile Fun pFun = argv[1][0] - '0' ? helper1 : helper2;
  Fun tmp = helper3;
  WRITE_TRACE("Func pointer before modified: 0x", pFun);
  SET_MEM(&pFun, tmp);
  WRITE_TRACE("Func pointer after modified: 0x", pFun);
  pFun();
  if(0 == gvar()) return RT_CODE_SUCCESS;  // helper3 ran: the attack succeeded
  return 1;                                // a wrong helper was reached
}
