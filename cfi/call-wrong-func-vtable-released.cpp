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
  #if defined(_MSC_VER)
    begin_catch_exception((void *)NULL, (ULONG*)NULL, (ULONG)17, (ULONG)EXCEPTION_IN_PAGE_ERROR);
    begin_catch_exception((void *)NULL, (ULONG*)NULL, (ULONG)18, (ULONG)EXCEPTION_ACCESS_VIOLATION);
  #else
    begin_catch_exception((void *)NULL, (int)SEGV_MAPERR, 17);
    begin_catch_exception((void *)NULL, (int)SEGV_ACCERR, 18);
  #endif
  orig->virtual_func();
  end_catch_exception();
  end_catch_exception();
  delete orig;
  return 4;
}


