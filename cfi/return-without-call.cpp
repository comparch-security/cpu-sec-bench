#include <cstdlib>
#include "include/global_var.hpp"
#include "include/signal.hpp"

extern "C"
FORCE_NOINLINE void helper(arch_int_t fsize) {
  void *exit_label = (void*)&helper;
  GET_RAA_SP_OFFSET(fsize);
  GET_LABEL_ADDRESS(exit_label,TARGET_LABEL);
  if(2 == gvar()) { GOTO_SAVED_LABEL(exit_label);}   // impossible to happen
  #ifdef TRACE_RUN
    GET_RA_ADDR(ra_addr);
    WRITE_TRACE("RA address: 0x", ra_addr);
    WRITE_TRACE("RA before modified: 0x", *(long long*)ra_addr);
  #endif
  PUSH_FAKE_RET(exit_label, fsize);
  WRITE_TRACE("RA address: 0x", ra_addr);
  WRITE_TRACE("RA after modified: 0x", *(long long*)ra_addr);
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
