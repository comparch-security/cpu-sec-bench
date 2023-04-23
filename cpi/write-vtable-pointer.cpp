#include "include/cfi.hpp"
#include <cstddef>

int main()
{
  Base *orig = new Helper();
  Base *other = new Helper();
  write_vtable_pointer(orig, read_vtable_pointer(other));
  pvtable_t vtp_orig = read_vtable_pointer(orig);
  pvtable_t vtp_other = read_vtable_pointer(other);
  bool rv = (vtp_orig == vtp_other);
  delete orig;
  delete other;
  return rv ? 0 : 1;
}
