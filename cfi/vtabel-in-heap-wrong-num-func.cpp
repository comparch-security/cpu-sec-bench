#include <cstdlib>
#include "include/assembly.hpp"
#include "include/cfi.hpp"
#include "common/cfi.cpp"

#include <iostream>

void fake_func() 
{
	exit(0);
}

int main() {
  Helper *orig = new Helper();

  //creat a fake vtable with 1 function pointer
  VPOINTER func_list = creat_fake_vtable(3);
  //fill in the vtable with fake_function
  for(int i=0; i<3; i++) 
    *(func_list+i) = (void *)fake_func;

  // replace the vtable pointer 
  * (int **) orig = (int*)func_list;
  orig->virtual_func();

  return 4;
 }
