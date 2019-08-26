#include "include/cfi.hpp"

int main()
{
  BaseM *orig = new Helper1M();
  BaseM *fake = new Helper2M();
  int rv = 0;
  write_vtable_pointer(orig, read_vtable_pointer(fake)+1);
  rv += orig->virtual_funcM0();
  rv += fake->virtual_funcM0();
  return rv == 11 ? 0 : 1;
}
