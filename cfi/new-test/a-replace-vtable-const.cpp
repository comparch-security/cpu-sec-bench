#include <cstdlib>
#include "include/assembly.hpp"
#include "include/cfi.hpp"
#include "common/cfi.cpp"

void fake_func() 
{
	exit(0);
}

void test( Helper *a)
{
     a->virtual_func();
}

  //void(*f[1])(void) = {fake_func};

  pvtable_t fake_vtable;

  //*fake_vtable = (pfunc_t)fake_func;


int main() {
  Helper *orig = new Helper();
  
  *fake_vtable = (pfunc_t)fake_func;
  //pvtable_t fake_vtable[1];

  //fake_vtable[0] = (pfunc_t)virtual_func;

  // replace the vtable pointer 

  XCHG_MEM(orig,&fake_vtable);

  // call the original virtual function
  //orig->virtual_func();


  test(orig);

  return 4;
 }