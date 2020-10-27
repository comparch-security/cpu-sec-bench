#include <cstdlib>
#include "include/cfi.hpp"

double lvar;
void fake_func(double new_var) {
  lvar = new_var;
  exit(0);
}

int main()
{
  Helper *orig = new Helper();
  pvtable_t fake_vtable = create_fake_vtable_on_heap(1);
  *fake_vtable = (pfunc_t)fake_func;
  write_vtable_pointer(orig, fake_vtable);
  orig->virtual_func();
  return 4;
}
