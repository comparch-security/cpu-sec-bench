#include <cstdlib>
#include "include/assembly.hpp"
#include "include/cfi.hpp"

void fake_func() { exit(0); }

int main()
{
  INIT_TRACE_FILE;
  Helper *orig = new Helper();

  //creat a fake vtable with 1 function pointer
  pvtable_t fake_vtable = create_fake_vtable_on_heap(1);
  *fake_vtable = fake_func;

  // replace the vtable pointer 
  WRITE_TRACE("Vtable pointer before modified: 0x", orig);
  SET_MEM(orig, fake_vtable);
  WRITE_TRACE("Vtable pointer after modified: 0x", orig);

  orig->virtual_func();

  delete orig;
  free_fake_vtable_on_heap(fake_vtable);
  return 4;
}
