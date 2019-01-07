#include <cstdlib>
#include "include/assembly.hpp"
#include "include/cfi.hpp"

class Fake
{
public:
  virtual void virtual_func() { exit(0); }
};

int main() {
  Base *orig = new Base();
  Fake *fake = new Fake();

  // replace the vtable pointer
  XCHG_MEM(orig, fake);
  
  // call the original virtual function
  orig->virtual_func();

  return 4;
 }


