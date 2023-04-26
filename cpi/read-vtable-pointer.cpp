#include "include/cfi.hpp"
#include <cstddef>

Helper data_helper;
Base*  data_orig = &data_helper;

const Helper const_data_helper;
const Base*  const_data_orig = &const_data_helper;

int main(int argc, char** argv)
{
  int flag_option = argv[1][0] - '0';

  Helper stack_helper1;
  Base*  stack_orig = &stack_helper1;

  Base *heap_orig = new Helper();

  pvtable_t vtp = NULL;
  switch (flag_option)
  {
  case 0:
    vtp = read_vtable_pointer(stack_orig);
    break;
  case 1:
    vtp = read_vtable_pointer(heap_orig);
    break;
  case 2:
    vtp = read_vtable_pointer(data_orig);
    break;
  case 3:
    vtp = read_vtable_pointer(const_data_orig);
    break;
  default:
    break;
  }

  bool rv = (vtp == NULL);
  delete heap_orig;
  return rv ? 1 : 0;
}
