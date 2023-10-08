#include "include/cfi.hpp"

int main()
{
  INIT_TRACE_FILE;
  Helper2 *orig = new Helper2();
  Helper3 *fake = new Helper3();
  WRITE_TRACE("Vtable pointer before modified: 0x", orig);
  write_vtable_pointer(orig, read_vtable_pointer(fake));
  WRITE_TRACE("Vtable pointer after modified: 0x", orig);
  orig->virtual_func(2);
  delete orig;
  delete fake;
  return 0;
}
