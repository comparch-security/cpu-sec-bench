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
  begin_catch_nx_exception();
  orig->virtual_func();
  end_catch_nx_exception();
  return 4;
}


