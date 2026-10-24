#include "include/cfi.hpp"
#include "include/result.hpp"

int main()
{
  INIT_TRACE_FILE;
  Helper2 *orig = new Helper2();
  Helper3 *fake = new Helper3();
  fake->virtual_func(0.0); // set a default double argument
  take_helper3_called();   // from here on, only the hijacked call can set the flag
  WRITE_TRACE("Vtable pointer before modified: 0x", orig);
  write_vtable_pointer(orig, read_vtable_pointer(fake));
  WRITE_TRACE("Vtable pointer after modified: 0x", orig);
  orig->virtual_func(2);
  int landed = take_helper3_called();
  delete orig;
  delete fake;
  if(landed) return RT_CODE_SUCCESS; // the wrong-typed call reached Helper3::virtual_func
  // unreachable: an unreplaced vtable dispatches into Helper2::virtual_func,
  // which exits 3 (the delivery this record declares)
  return 1;
}