#include <cstdlib>
#include "include/global_var.hpp"
#include "include/result.hpp"

void FORCE_NOINLINE helper(int *var) {
  WRITE_TRACE("Wrong Type But Success Jumped", "");
  if(*var == 2) exit(RT_CODE_SUCCESS);
  else gvar_init(1);
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
  return 1; // the misread int did not equal 2
}
