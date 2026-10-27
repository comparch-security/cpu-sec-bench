#include <cstdlib>
#include "include/global_var.hpp"
#include "include/signal.hpp"

extern "C"
FORCE_NOINLINE void helper(arch_int_t fsize) {
  void *exit_label = (void*)&helper;
  GET_RAA_SP_OFFSET(fsize);
  GET_LABEL_ADDRESS(exit_label,TARGET_LABEL);
  if(2 == gvar()) { GOTO_SAVED_LABEL(exit_label);}   // impossible to happen
  /* The whole locator is TRACE_RUN-only: outside it the scan only reads dead
   * values, and the declarations must not sit in the path of the computed
   * goto above -- clang rejects a jump that bypasses an initialised variable,
   * so they carry no initialisers and are assigned inside the region.
   */
  #ifdef TRACE_RUN
    /* The trace address follows this frame's shape, not the calibrated frame
     * size: `fsize` is the producer's word count (and GET_RAA_SP_OFFSET is a
     * no-op on POSIX), so it names neither a byte offset nor this function's
     * slot.  Locate the slot by the value it holds -- this frame's own return
     * address -- and trace nothing when it is not found.
     */
    void *ra_val;
    GET_RETURN_ADDRESS(ra_val);
    void *ra_stack;
    arch_int_t ra_offset;
    ra_stack = NULL;
    ra_offset = -1;
    #define RA_POS_TEST_ENTRY(off, rv)                    \
      READ_STACK_DAT_IMM(ra_stack, off);                  \
      if(ra_offset < 0 && STRIP_PAC(ra_stack) == ra_val) ra_offset = off;
    RA_POS_TEST
    #undef RA_POS_TEST_ENTRY
    if(ra_offset >= 0) {
      ra_addr = ra_offset;
      GET_RA_ADDR(ra_addr);
      WRITE_TRACE("RA address: 0x", ra_addr);
      WRITE_TRACE("RA before modified: 0x", *(long long*)ra_addr);
    }
  #endif
  PUSH_FAKE_RET(exit_label, fsize);
  #ifdef TRACE_RUN
    if(ra_offset >= 0) {
      WRITE_TRACE("RA address: 0x", ra_addr);
      WRITE_TRACE("RA after modified: 0x", *(long long*)ra_addr);
    }
  #endif
  if(fsize != 0)return;

TARGET_LABEL(fake_use_arg)
  gvar_decr();
  exit(RT_CODE_SUCCESS); // the hijacked RA reached the fake return site
}

int main(int argc, char* argv[])
{
  INIT_TRACE_FILE;
  gvar_init(argv[1][0] - '0');
  // a literal -v token means the calibration variable was never recorded; parse the
  // whole token, since a produced frame size can be two digits
  char *end = NULL;
  long fsize_num = strtol(argv[2], &end, 10);
  if(end == argv[2] || *end != '\0') return 1;
  arch_int_t fsize = fsize_num * 4 / sizeof(arch_int_t);
  begin_catch_exception((void *)NULL, BUS_ADRALN, RT_CODE_ACCERR, SIGBUS);
  helper(fsize);
  end_catch_exception();
  return 1; // the RA was not pushed out of the frame
}
