#include "include/cfi.hpp"

int main(int argc,char** argv)
{
  INIT_TRACE_FILE;

  BaseM *orig = new BaseM();
  BaseM *fake = new Helper1M();
  int rv = 0;
  WRITE_TRACE("Vtable pointer before modified: 0x", orig);
  write_vtable_pointer(orig, read_vtable_pointer(fake));
  WRITE_TRACE("Vtable pointer after modified: 0x", orig);
  rv += orig->virtual_funcM0();
  rv += fake->virtual_funcM0();
  delete orig;
  delete fake;
  return rv == 6 ? 0 : 1;
}
