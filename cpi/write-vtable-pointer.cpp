#include "include/cfi.hpp"
#include <cstddef>

int main()
{
  Base *orig = new Helper();
  Base *other = new Helper();
  write_vtable_pointer(orig, read_vtable_pointer(other));
  pvtable_t vtp = read_vtable_pointer(orig);
  return (vtp == NULL) ? 1 : 0;
}
