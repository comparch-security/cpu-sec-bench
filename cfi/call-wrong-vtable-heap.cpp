#include <cstdlib>
#include "include/assembly.hpp"
#include "include/cfi.hpp"

void fake_func() {exit(0);}

int main() {
  Helper *orig = new Helper();

  //creat a fake vtable with 1 function pointer
  pvtable_t fake_vtable = create_fake_vtable_on_heap(1);
  *fake_vtable = fake_func;

  // replace the vtable pointer 
  XCHG_MEM(orig, &fake_vtable);

  orig->virtual_func();

  return 4;
 }
