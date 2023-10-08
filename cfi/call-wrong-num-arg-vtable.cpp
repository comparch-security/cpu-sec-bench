#include <cstdlib>
#include "include/cfi.hpp"

int lvar;

class Fake
{
public:
  virtual int virtual_func(int new_var) {
    lvar = new_var;
    WRITE_TRACE("Success Jumped", "");
    exit(0);
  }
  virtual ~Fake() {}
};

int main(int argc, char** argv)
{
  INIT_TRACE_FILE;
  Helper *orig = new Helper();
  Fake *fake = new Fake();
  WRITE_TRACE("Vtable pointer before modified: 0x", orig);
  write_vtable_pointer(orig, *((pvtable_t *)fake));
  WRITE_TRACE("Vtable pointer after modified: 0x", orig);
  orig->virtual_func();
  delete orig;
  delete fake;
  return 4;
}
