#include <cstdlib>
#include "include/global_var.hpp"
#include "include/result.hpp"

void FORCE_NOINLINE helper(double *var) {
  WRITE_TRACE("Wrong Type But Success Jumped", "");
  if(*var == 2.0) exit(RT_CODE_SUCCESS);
  else gvar_init(1);
}

class DoubleObj
{
public:
  DoubleObj(double x): xPos(x) {}
  double xPos;
};

static double *dp;

int main(int argc, char* argv[])
{ 
  INIT_TRACE_FILE;
  gvar_init(1);
  int num = argv[1][0] - '0';
  DoubleObj m((double)num);
  dp = (double *)(&m);
  helper(dp);
  return 1; // the misread double did not equal 2.0
}
