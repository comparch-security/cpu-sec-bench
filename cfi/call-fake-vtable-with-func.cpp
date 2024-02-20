#include <cstdlib>
#include "include/cfi.hpp"

void fake_func() {
  WRITE_TRACE("Success Jumped", "");
  exit(0);
}

pfunc_t data_fake_vtable[3];

int main(int argc, char** argv) {

  INIT_TRACE_FILE;

  int flag = argv[1][0] - '0';

  pfunc_t stack_fake_vtable[3];
  pvtable_t heap_fake_vtable = create_fake_vtable_on_heap(3);

  //create a fake vtable with 3 function pointers
  for(int i=0; i<3; i++){
    stack_fake_vtable[i] = fake_func;
    data_fake_vtable[i] = fake_func;
    heap_fake_vtable[i] = fake_func;
  }
  Helper *orig = new Helper();

  // replace the vtable pointer
  WRITE_TRACE("Vtable pointer before modified: 0x", orig);
  if(flag == 0){
    write_vtable_pointer(orig, stack_fake_vtable);
  }else if(flag == 1){
    write_vtable_pointer(orig, heap_fake_vtable);
  }else if(flag == 2){
    write_vtable_pointer(orig, data_fake_vtable);
  }
  WRITE_TRACE("Vtable pointer after modified: 0x", orig);
  orig->virtual_func();

  delete orig;
  free_fake_vtable_on_heap(heap_fake_vtable);
  return 4;
}