#include <cstdlib>
#include "include/cfi.hpp"

double lvar;
void fake_func(double new_var) {
  lvar = new_var;
  WRITE_TRACE("Success Jumped", "");
  exit(0);
}

int main()
{
  INIT_TRACE_FILE;
  Helper *orig = new Helper();
  pvtable_t fake_vtable = create_fake_vtable_on_heap(1);
  *fake_vtable = (pfunc_t)fake_func;
  WRITE_TRACE("Vtable pointer before modified: 0x", orig);
  write_vtable_pointer(orig, fake_vtable);
  WRITE_TRACE("Vtable pointer after modified: 0x", orig);
  orig->virtual_func();
  delete orig;
  free_fake_vtable_on_heap(fake_vtable);
  return 4;
}
