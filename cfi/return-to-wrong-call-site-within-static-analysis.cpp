#include <cstdlib>
#include "include/global_var.hpp"
#include <string>

volatile arch_int_t offset;

void FORCE_NOINLINE helper(void *label, int sel) {
  gvar_incr();
  void * buf[2];
  if(gvar() == 2) {
    /* On MAC M1 Clang 12.0.5
     * Due to the compiler optimization, offset is modified
     * in the MOD_STACK_DAT macro.
     * As a result, we borrow gvar to deliver the final offset
     * as used by the later exit() after EXIT_POS.
     */
    gvar_init(-offset/4);
    #ifdef TRACE_RUN
      GET_RA_ADDR(ra_addr);
      WRITE_TRACE("RA address: 0x", ra_addr);
      WRITE_TRACE("RA before modified: 0x", *(long long*)ra_addr);
    #endif
    if(sel){
      MOD_STACK_DAT(label, offset);
    }else{
      *(buf+offset) = label;
    }
    WRITE_TRACE("RA address: 0x", ra_addr);
    WRITE_TRACE("RA after modified: 0x", *(long long*)ra_addr);
  }
}

int main(int argc, char* argv[])
{
  INIT_TRACE_FILE;
  // get the offset of RA on stack

  int sel = std::stol(argv[1]);
  std::string cmd_var_offset = argv[2];
  std::string cmd_range_offset = argv[3];
  arch_int_t var_offset = stoll(cmd_var_offset);
  arch_int_t range_offset = stoll(cmd_range_offset);

  offset = 4*(range_offset + var_offset);
  void *ret_label = (void*)&main;
  GET_LABEL_ADDRESS(ret_label,TARGET_LABEL);
  gvar_init(0);
  if(offset == -1) { GOTO_SAVED_LABEL(ret_label);}   // impossible to happen

  // call a function but illegally return
  helper(ret_label,sel);
  COMPILER_BARRIER;
  // the elligal return site
TARGET_LABEL(argc)
  if(gvar() < 0) exit(32 - gvar());
  helper(ret_label,sel);
  helper(ret_label,sel);
  return gvar();
}
