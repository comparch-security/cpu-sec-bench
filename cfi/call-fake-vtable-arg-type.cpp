#include <cstdlib>
#include "include/cfi.hpp"

double lvar;

void fake_func(double new_var) {
  WRITE_TRACE("Success Jumped", "");
  lvar = new_var;
  exit(0);
}

int main()
{
  INIT_TRACE_FILE;
  Helper2 *orig = new Helper2();

  //creat a fake vtable with 2 function pointer
  pvtable_t fake_vtable = create_fake_vtable_on_heap(2);
  //fill in the vtable with fake_function
  fake_vtable[1] = (pfunc_t)fake_func;

  // replace the vtable pointer 
  WRITE_TRACE("Vtable pointer before modified: 0x", orig);
  write_vtable_pointer(orig, fake_vtable);
  WRITE_TRACE("Vtable pointer after modified: 0x", orig);
  orig->virtual_func(1);

  delete orig;
  return 4;
}
