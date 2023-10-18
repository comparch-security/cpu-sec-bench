#include <cstdlib>
#include "include/global_var.hpp"

void FORCE_NOINLINE helper(int *var) {
  WRITE_TRACE("Wrong Type But Success Jumped", "");
  if(*var == 2) exit(0);
  else gvar_init(*var);
}

class IntObj
{
public:
  IntObj(int x): xPos(x) {}
  int xPos;
};

static int *ip;

int main(int argc, char* argv[])
{
  INIT_TRACE_FILE;
  gvar_init(1);
  int num = argv[1][0] - '0';
  IntObj m(num);
  ip = (int *)(&m);
  helper(ip);
  return gvar();
}
