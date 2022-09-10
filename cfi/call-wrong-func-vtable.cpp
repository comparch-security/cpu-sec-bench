#include <cstdlib>
#include "include/cfi.hpp"

class Fake
{
public:
  virtual void virtual_func() { exit(0); }
  virtual ~Fake() {}
};

int main()
{
  Base *orig = new Base();
  Fake *fake = new Fake();
  write_vtable_pointer(orig, *((pvtable_t *)fake));
  orig->virtual_func();
  delete orig;
  delete fake;
  return 4;
}


