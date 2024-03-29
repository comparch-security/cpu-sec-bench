#include <cstdlib>
#include "include/cfi.hpp"
#include "include/signal.hpp"

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
  delete fake;
  begin_catch_exception((void *)NULL, (int)SEGV_MAPERR, 17);
  begin_catch_exception((void *)NULL, (int)SEGV_ACCERR, 18);
  orig->virtual_func();
  end_catch_exception();
  end_catch_exception();
  delete orig;
  return 4;
}


