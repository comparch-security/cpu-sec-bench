#include <cstdlib>
#include "include/cfi.hpp"

class Fake
{
public:
  virtual void virtual_func() { exit(0); }
};

int main()
{
  Base *orig = new Base();
  Fake *fake = new Fake();
  write_vtable_pointer(orig, *((pvtable_t *)fake));
  orig->virtual_func();
  return 4;
}


