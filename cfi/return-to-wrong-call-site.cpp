#include <cstdlib>
#include "include/global_var.hpp"
#include <string>

volatile arch_int_t offset;

void FORCE_NOINLINE helper(void *label) {
  gvar_init(3);
  COMPILER_BARRIER;
  GET_RAA_SP_OFFSET(offset);
  #ifdef TRACE_RUN
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
   * the -within-analysis test.
   */
  offset = rand();
  gvar_init(0);
}

void FORCE_NOINLINE helper2() {
  gvar_init(4);
}

int main(int argc, char* argv[])
{
  INIT_TRACE_FILE;
  // get the offset of RA on stack
  if(argc == 2){
    std::string cmd_offset = argv[1];
    offset = 4 * stoll(cmd_offset);
  }else{
    std::string cmd_offset = argv[1];
    std::string extra_offset = argv[2];
    offset = 4*(stoll(cmd_offset) + stoll(extra_offset));
  }
  void *ret_label = (void*)&main;
  GET_LABEL_ADDRESS(ret_label,TARGET_LABEL);
  if(offset == -1) { GOTO_SAVED_LABEL(ret_label);}   // impossible to happen

  // call a function but illegally return
  helper(ret_label);
  helper2();// failed if runs here
  COMPILER_BARRIER;
  // the elligal return site
TARGET_LABEL(argc)
  exit(gvar());
}
