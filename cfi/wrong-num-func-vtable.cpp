#include <cstdlib>
#include "include/assembly.hpp"
#include "include/cfi.hpp"

class HelperReplace : public HelperOrig
{
public:
  virtual void virtual_func() { exit(0); } // fake virtual function
  virtual void other_func() { exit(2); }
};

int main() {
  HelperOrig orig, *pOrig = &orig;
  HelperReplace replace;

  // replace the vtable pointer
  XCHG_MEM(&orig, &replace);
  pOrig->virtual_func();

  return 3;
}

