#include <cstdlib>
#include "include/assembly.hpp"
#include "include/cfi.hpp"

double lvar;
void fake_func(double new_var) 
{
	lvar = new_var;
	exit(0);
}

int main() {
  Helper *orig = new Helper();

  //creat a fake vtable with 1 function pointer
  pvtable_t fake_vtable = create_fake_vtable_on_heap(1);
  *fake_vtable = (pfunc_t)fake_func;

  // replace the vtable pointer 
  * (int **) orig = (int*)fake_vtable;
  orig->virtual_func();

  return 4;
 }
