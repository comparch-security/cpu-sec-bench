#include "include/cfi.hpp"

int main()
{
  BaseM *orig = new Helper1M();
  BaseM *fake = new Helper1M();
  int rv = 0;
  write_vtable_pointer(orig, read_vtable_pointer(fake)+1);
  rv += orig->virtual_funcM0();
  rv += fake->virtual_funcM0();
  delete orig;
  delete fake;
  return rv == 7 ? 0 : 1;
}
