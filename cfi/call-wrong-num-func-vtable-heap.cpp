#include <cstdlib>
#include "include/cfi.hpp"

void fake_func() {
  exit(0);
}

int main()
{
  Helper *orig = new Helper();

  //create a fake vtable with 3 function pointers
  pvtable_t fake_vtable = create_fake_vtable_on_heap(3);
  for(int i=0; i<3; i++)
    fake_vtable[i] = fake_func;

  // replace the vtable pointer 
  write_vtable_pointer(orig, fake_vtable);
  orig->virtual_func();

  delete orig;
  free_fake_vtable_on_heap(fake_vtable);
  return 4;
}
