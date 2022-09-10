#include "include/cfi.hpp"
#include <cstddef>

int main()
{
  Base *orig = new Helper();
  pvtable_t vtp = read_vtable_pointer(orig);
  bool rv = (vtp == NULL);
  delete orig;
  return rv ? 1 : 0;
}
