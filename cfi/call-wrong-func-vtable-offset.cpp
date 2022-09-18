#include "include/cfi.hpp"
#include <cstdlib>

int main()
{
  BaseM *orig = new Helper1M();
  BaseM *fake = new Helper1M();
  int rv = 0;
  write_vtable_pointer(orig, read_vtable_pointer(fake)+1);
  rv += orig->virtual_funcM0();
  rv += fake->virtual_funcM0();
  /* The following delete operations are commented out
     as they cause segment error on tests compiled by G++ 11.2.0
  */
  // delete orig;
  // delete fake;

  // use exit instead of return, try to avoid memory leak detection in ASan
  exit(rv == 7 ? 0 : 1);
}
