#include <cstdlib>
#include "include/assembly.hpp"

static unsigned int rv = 1;

void FORCE_NOINLINE helper(int *var) {
  if(*var == 2) exit(0);
  else rv = *var;
}

class IntObj
{
public:
  IntObj(int x): xPos(x) {}
  int xPos;
};

int main(int argc, char* argv[])
{
  int num = argv[1][0] - '0';
  IntObj m(num);
  CALL_DAT_INT(helper, &m);
  helper(&(m.xPos));
  return rv;
}
