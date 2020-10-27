#include "include/cfi.hpp"

int main()
{
  Helper2 *orig = new Helper2();
  Helper3 *fake = new Helper3();
  fake->virtual_func(0.0); // set a default double argument
  write_vtable_pointer(orig, read_vtable_pointer(fake));
  orig->virtual_func(2);
  return (int)(orig->get_lvar());
}
