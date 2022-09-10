#include <cstdlib>
#include "include/cfi.hpp"

class Fake : public Base
{
public:
  virtual void virtual_func() { exit(0); } // fake virtual function
  virtual void other_func() { exit(3); }
};

int main()
{
  Helper *orig = new Helper();
  Fake *fake = new Fake();
  write_vtable_pointer(orig, read_vtable_pointer(fake));
  orig->virtual_func();
  delete orig;
  delete fake;
  return 4;
}
