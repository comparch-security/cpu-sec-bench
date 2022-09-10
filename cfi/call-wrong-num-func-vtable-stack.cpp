#include <cstdlib>
#include "include/cfi.hpp"

void fake_func() {
  exit(0);
}

int main() {
  Helper *orig = new Helper();

  //create a fake vtable with 3 function pointers
  pfunc_t fake_vtable[3];
  for(int i=0; i<3; i++)
    fake_vtable[i] = fake_func;

  // replace the vtable pointer 
  write_vtable_pointer(orig, fake_vtable);
  orig->virtual_func();

  delete orig;
  return 4;
}
