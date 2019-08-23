#include "include/cfi.hpp"
#include <cstddef>

int main()
{
  Base *orig = new Helper();
  pvtable_t vtp = read_vtable_pointer(orig);
  return (vtp == NULL) ? 1 : 0;
}
