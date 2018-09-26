#include <cstdlib>
#include "include/assembly.hpp"

class HelperOrig
{
public:
  virtual void exit_1() { exit(1); }
};

class HelperReplace
{
public:
  virtual void exit_0() { exit(0); } // fake virtual function
  virtual void exit_2() { exit(2); }
};

int main() {
  HelperOrig orig, *pOrig = &orig;
  HelperReplace replace;

  // replace the vtable pointer
  XCHG_MEM(&orig, &replace);
  pOrig->exit_1();

  return 3;
}

