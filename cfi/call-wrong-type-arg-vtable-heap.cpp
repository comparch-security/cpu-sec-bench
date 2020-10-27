#include <cstdlib>
#include "include/cfi.hpp"

double lvar;

void fake_func(double new_var) {
  lvar = new_var;
  exit(0);
}

int main()
{
  Helper2 *orig = new Helper2();

  //creat a fake vtable with 2 function pointer
  pvtable_t fake_vtable = create_fake_vtable_on_heap(2);
  //fill in the vtable with fake_function
  fake_vtable[1] = (pfunc_t)fake_func;

  // replace the vtable pointer 
  write_vtable_pointer(orig, fake_vtable);
  orig->virtual_func(1);

  return 4;
}
