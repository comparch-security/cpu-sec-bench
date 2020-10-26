#include "include/cfi.hpp"

int main()
{
  BaseM *orig = new BaseM();
  BaseM *fake = new Helper1M();
  int rv = 0;
  write_vtable_pointer(orig, read_vtable_pointer(fake));
  rv += orig->virtual_funcM0();
  rv += fake->virtual_funcM0();
  return rv == 6 ? 0 : 1;
}
