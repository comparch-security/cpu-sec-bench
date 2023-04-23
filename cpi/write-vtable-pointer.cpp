#include "include/cfi.hpp"
#include <cstddef>

Helper data_helper1;
Helper data_helper2;
Base*  data_orig = &data_helper1;
Base*  data_other = &data_helper2;

bool check_vtable_write(Base* orig, Base* other){

  write_vtable_pointer(orig, read_vtable_pointer(other));
  pvtable_t vtp_orig = read_vtable_pointer(orig);
  pvtable_t vtp_other = read_vtable_pointer(other);
  return (vtp_orig == vtp_other);
}

int main(int argc, char** argv)
{
  int flag_option = argv[1][0]-'0';

  Helper stack_helper1;
  Helper stack_helper2;
  Base*  stack_orig = &stack_helper1;
  Base*  stack_other = &stack_helper2;

  Base *heap_orig = new Helper();
  Base *heap_other = new Helper();

  bool rv = 1;
  switch (flag_option)
  {
  case 0:
    rv = check_vtable_write(stack_orig, stack_other);
    break;
  case 1:
    rv = check_vtable_write(heap_orig, heap_other);
    break;
  case 2:
    rv = check_vtable_write(data_orig, data_other);
    break;
  default:
    break;
  }

  delete heap_orig;
  delete heap_other;
  return rv ? 0 : 1;
}
