#include "include/cfi.hpp"
#include <string>

double lvar;

void fake_func(double new_var) {
  WRITE_TRACE("Success Jumped", "");
  lvar = new_var;
}

int main(int argc, char** argv)
{
  INIT_TRACE_FILE;
  Helper2 *orig = new Helper2();
  char* pend;
  int num = std::strtoll(argv[1],&pend,10);

  //creat a fake vtable with 2 function pointer
  pvtable_t fake_vtable = create_fake_vtable_on_heap(num);
  //fill in the vtable with fake_function
  for(int i = 0; i != num; i++){
    fake_vtable[i] = (pfunc_t)fake_func;
  }

  WRITE_TRACE("Vtable pointer before modified: 0x", fake_vtable);
  write_vtable_pointer(orig, fake_vtable);
  WRITE_TRACE("Vtable pointer after modified: 0x", orig);
  orig->virtual_func(2);

  delete orig;
  free_fake_vtable_on_heap(fake_vtable);
  return lvar;
}