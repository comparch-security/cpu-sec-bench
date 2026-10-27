#include <cstdlib>
#include <cstring>
#include <cstdint>
#include "include/cfi.hpp"
#include "include/global_var.hpp"
#include <string>

volatile arch_int_t offset = 0;

void FORCE_NOINLINE helper(void* label) {
  gvar_init(0);
  GET_RAA_SP_OFFSET(offset);
  #ifdef TRACE_RUN
    ra_addr = offset;
    GET_RA_ADDR(ra_addr);
    WRITE_TRACE("RA address: 0x", ra_addr);
    WRITE_TRACE("RA before modified: 0x", *(long long*)ra_addr);
  #endif
  MOD_STACK_DAT(label, offset);
  WRITE_TRACE("RA address: 0x", ra_addr);
  WRITE_TRACE("RA after modified: 0x", *(long long*)ra_addr);
  /* HiFive Unmatched, GCC 11.2.0
   * Make sure offset is modified as otherwise
   * the stack is not expanded similarily with
   * the return-to-parent-same-call-site test.
   */
  offset = rand();
}

int main(int argc, char* argv[])
{
  INIT_TRACE_FILE;
  Ret_To_Helper* ptr = new Ret_To_Helper();
  // get the offset of RA on stack
  // a literal -v token means the calibration variable was never recorded
  char *end = NULL;
  long offset_num = strtol(argv[1], &end, 10);
  if(end == argv[1] || *end != '\0') return 1;
  offset = 4 * offset_num;
  uintptr_t* fake_ret_ptr = reinterpret_cast<uintptr_t *>(ptr);
  uintptr_t* fake_ret_adr = reinterpret_cast<uintptr_t *>(*fake_ret_ptr);
  void* fake_ret = reinterpret_cast<void*>(*fake_ret_adr);
  helper(fake_ret);
  return 1;
}