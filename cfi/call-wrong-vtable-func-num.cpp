#include <cstdlib>
#include "include/cfi.hpp"

class Fake : public Base
{
public:
  virtual void virtual_func() {   WRITE_TRACE("Success Jumped", ""); exit(0); } // fake virtual function
  virtual void other_func() {   WRITE_TRACE("Fail Jumped", ""); exit(3); }
};

int main(int argc, char** argv)
{
  INIT_TRACE_FILE;
  Helper *orig = new Helper();
  Fake *fake = new Fake();
  WRITE_TRACE("Vtable pointer before modified: 0x", orig);
  write_vtable_pointer(orig, read_vtable_pointer(fake));
  WRITE_TRACE("Vtable pointer after modified: 0x", orig);
  orig->virtual_func();
  delete orig;
  delete fake;
  return 4;
}
