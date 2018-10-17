#include <cstdlib>
#include "include/assembly.hpp"
#include "include/cfi.hpp"
#include "common/cfi.cpp"

#include <iostream>

double lvar;
void fake_func(double new_var) 
{
	lvar = new_var;
	exit(0);
}

int main() {
  Helper2 *orig = new Helper2();

  //creat a fake vtable with 2 function pointer
  //
  VPOINTER func_list = creat_fake_vtable(2);
  //fill in the vtable with fake_function
  *func_list = (void *)fake_func;
  *(func_list+1) = (void *)fake_func;

  // replace the vtable pointer 
  * (int **) orig = (int*)func_list;
  orig->virtual_func(1);

  return 4;
 }
