#include "include/cfi.hpp"
#include <cstdlib>

int main(int argc, char** argv)
{
  INIT_TRACE_FILE;
  bool fake_flag = argv[1][0] - '0';
  BaseM *orig = new Helper1M();
  BaseM *fake = new Helper1M();
  int rv = 0;
  WRITE_TRACE("Vtable pointer before modified: 0x", orig);
  write_vtable_pointer(orig, read_vtable_pointer(fake)+1);
  WRITE_TRACE("Vtable pointer after modified: 0x", orig);
  rv += orig->virtual_funcM0();
  rv += fake->virtual_funcM0();
  /* The following delete operations are commented out
     as they cause segment error on tests compiled by G++ 11.2.0,
     because the content of orig was crashed
  */

  // use exit instead of return, try to avoid memory leak detection in ASan
  if(!fake_flag) exit(rv == 7 ? 0 : 1);

  delete orig;
  delete fake;

  return 0;
}
