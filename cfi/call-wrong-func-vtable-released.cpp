#include <cstdlib>
#include "include/cfi.hpp"
#include "include/signal.hpp"

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
  delete fake;
  write_vtable_pointer(orig, *((pvtable_t *)fake));
  begin_catch_exception((void *)NULL, (int)SEGV_MAPERR, 1);
  orig->virtual_func();
  end_catch_exception();
  return 4;
}


