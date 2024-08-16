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
    GET_SP_BASE(ra_addr);
    WRITE_TRACE("RA address: 0x", ra_addr);
    WRITE_TRACE("RA before modified: 0x", *(long long*)ra_addr);
  #endif
  PUSH_FAKE_RET(exit_label, fsize);
  WRITE_TRACE("RA address: 0x", ra_addr);
  WRITE_TRACE("RA after modified: 0x", *(long long*)ra_addr);
  if(fsize != 0)return;

TARGET_LABEL(fake_use_arg)
  gvar_decr();
  exit(gvar());
}

int main(int argc, char* argv[])
{
  INIT_TRACE_FILE;
  gvar_init(argv[1][0] - '0');
  arch_int_t fsize = (argv[2][0] - '0') * 4 / sizeof(arch_int_t);
  begin_catch_exception((void *)NULL, BUS_ADRALN, RT_CODE_ACCERR, SIGBUS);
  helper(fsize);
  end_catch_exception();
  return gvar();
}
