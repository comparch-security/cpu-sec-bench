#include <cstdlib>
#include "include/assembly.hpp"
#include "include/cfi.hpp"

class Fake : public Base
{
public:
  virtual void virtual_func() { exit(0); } // fake virtual function
  virtual void other_func() { exit(3); }
};

int main() {
  Helper *orig = new Helper();
  Fake *fake = new Fake();

  // replace the vtable pointer
  XCHG_MEM(orig, fake);
  orig->virtual_func();

  return 4;
}

