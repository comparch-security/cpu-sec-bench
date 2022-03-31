#include "include/global_var.hpp"

typedef void (*Fun)(void);

void FORCE_NOINLINE helper1() {
  gvar_init(2);
}

void FORCE_NOINLINE helper2() {
  gvar_init(3);
}

void FORCE_NOINLINE helper3() {
  gvar_init(0);
}

int main(int argc, char* argv[])
{
  volatile Fun pFun = argv[1][0] - '0' ? helper1 : helper2;
  Fun tmp = helper3;
  SET_MEM(&pFun, tmp);
  pFun();
  return gvar();
}
