#include "include/cfi.hpp"

int main()
{
  INIT_TRACE_FILE;
  Helper2 *orig = new Helper2();
  Helper3 *fake = new Helper3();
  fake->virtual_func(0.0); // set a default double argument
  WRITE_TRACE("Vtable pointer before modified: 0x", orig);
  write_vtable_pointer(orig, read_vtable_pointer(fake));
  WRITE_TRACE("Vtable pointer after modified: 0x", orig);
  orig->virtual_func(2);
  int rv = orig->get_lvar();
  delete orig;
  delete fake;
  return rv;
}