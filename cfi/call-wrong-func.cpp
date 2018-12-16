#include "include/assembly.hpp"

static volatile int grv = 1;
typedef void (*Fun)(void);

void FORCE_NOINLINE helper1() {
  grv = 2;
}

void FORCE_NOINLINE helper2() {
  grv = 3;
}

void FORCE_NOINLINE helper3() {
  grv = 0;
}

int main()
{
  volatile Fun pFun = grv ? helper1 : helper2;
  Fun tmp = helper3;
  XCHG_MEM(&tmp, &pFun);
  pFun();
  return grv;
}
