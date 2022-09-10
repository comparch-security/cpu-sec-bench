#include "include/cfi.hpp"

int main()
{
  Helper2 *orig = new Helper2();
  Helper3 *fake = new Helper3();
  write_vtable_pointer(orig, read_vtable_pointer(fake));
  orig->virtual_func(2);
  delete orig;
  delete fake;
  return 0;
}
