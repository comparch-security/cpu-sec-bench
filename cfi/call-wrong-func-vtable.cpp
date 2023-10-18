#include <cstdlib>
#include "include/cfi.hpp"

class Fake
{
public:
  virtual void virtual_func() { exit(0); }
  virtual ~Fake() {}
};

int main(int argc, char** argv)
{
  INIT_TRACE_FILE;
  Base *orig = new Base();
  Fake *fake = new Fake();
  WRITE_TRACE("Vtable pointer before modified: 0x", orig);
  write_vtable_pointer(orig, *((pvtable_t *)fake));
  WRITE_TRACE("Vtable pointer after modified: 0x", orig);
  orig->virtual_func();
  delete orig;
  delete fake;
  return 4;
}


