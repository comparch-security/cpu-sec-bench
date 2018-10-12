#include <cstdlib>
#include "include/assembly.hpp"
#include "include/cfi.hpp"
#include "common/cfi.cpp"

#include <iostream>

void fake_func() {exit(0);}

int main() {
  Helper *orig = new Helper();

  //creat a fake vtable with 1 function pointer
  VPOINTER fake_vtable = creat_fake_vtable(1);
  *fake_vtable = (void *)fake_func;

  // replace the vtable pointer 
  XCHG_MEM(orig, &fake_vtable);
  //* (int **) orig = (int*)func_list;


  orig->virtual_func();

  return 4;
 }
