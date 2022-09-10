#include "include/cfi.hpp"

int main()
{
  BaseM *orig = new Helper1M();
  BaseM *fake = new BaseM();
  int rv = 0;
  write_vtable_pointer(orig, read_vtable_pointer(fake));
  rv += orig->virtual_funcM0();
  rv += fake->virtual_funcM0();
  delete orig;
  delete fake;
  return rv == 2 ? 0 : 1;
}
