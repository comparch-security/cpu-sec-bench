#include <cstdlib>
#include "include/assembly.hpp"
#include "include/cfi.hpp"

class Fake : public Base
{
public:
  virtual void virtual_func() { exit(0); }
};

int main()
{

  Base *orig = new Base();
  Fake *fake = new Fake();
  write_vtable_pointer(orig, read_vtable_pointer(fake));
  orig->virtual_func();
  return 4;
}
