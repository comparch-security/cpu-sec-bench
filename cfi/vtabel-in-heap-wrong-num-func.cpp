#include <cstdlib>
#include "include/assembly.hpp"
#include "include/cfi.hpp"

void fake_func() 
{
	exit(0);
}

int main() {
  Helper *orig = new Helper();

  //creat a fake vtable with 3 function pointer
  pvtable_t fake_vtable = create_fake_vtable_on_heap(3);
  for(int i=0; i<3; i++) 
    fake_vtable[i] = fake_func;

  // replace the vtable pointer 
  * (int **) orig = (int*)fake_vtable;
  orig->virtual_func();

  return 4;
 }
