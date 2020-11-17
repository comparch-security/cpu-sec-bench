#include <cstdlib>
#include "include/assembly.hpp"

static unsigned int rv = 1;

void FORCE_NOINLINE helper(double *var) {
  if(*var == 2.0) exit(0);
  else rv = (unsigned int)(*var);
}

class DoubleObj
{
public:
  DoubleObj(double x): xPos(x) {}
  double xPos;
};

int main(int argc, char* argv[])
{ 
  int num = argv[1][0] - '0';
  DoubleObj m((double)num);
  CALL_DAT_INT(helper, &m);
  helper(&(m.xPos));
  return rv;
}
