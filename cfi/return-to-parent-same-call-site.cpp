#include <cstdlib>
#include "include/global_var.hpp"
#include <string>

volatile arch_int_t offset;
volatile arch_int_t fake_val;
void* FORCE_NOINLINE helper(void *label, int get_offset_method, int get_ra_method){
  void* curr_return_addr = NULL;
  if(get_ra_method == 1){
    GET_RA_ADDR(curr_return_addr);
  }
  gvar_incr();
  volatile void * buf[2];
  if(gvar() == 2) {
    /* On MAC M1 Clang 12.0.5
     * Due to the compiler optimization, offset is modified
     * in the MOD_STACK_DAT macro.
     * As a result, we borrow gvar to deliver the final offset
     * as used by the later exit() after EXIT_POS.
     */
    // fake_val is 2, just used to depress optimization
    #ifdef TRACE_RUN
      GET_SP_BASE(ra_addr);
      WRITE_TRACE("RA address: 0x", ra_addr);
      WRITE_TRACE("RA before modified: 0x", *(long long*)ra_addr);
    #endif
    if(get_offset_method){
      MOD_STACK_DAT(label, offset);
      gvar_init(fake_val);
    }else{
      *(buf+offset) = label;
      gvar_init(fake_val/2+1);
    }
    WRITE_TRACE("RA address: 0x", ra_addr);
    WRITE_TRACE("RA after modified: 0x", *(long long*)ra_addr);
    return NULL;
  }else {
    if(get_ra_method == 1){
        return curr_return_addr;
    }else{
        return label;
    }
  }
}

int main(int argc, char* argv[])
{
  INIT_TRACE_FILE;
  // get the offset of RA on stack

  int offset_sel = std::stol(argv[1]);
  std::string cmd_var_offset = argv[2];
  std::string cmd_range_offset = argv[3];
  arch_int_t var_offset = stoll(cmd_var_offset);
  arch_int_t range_offset = stoll(cmd_range_offset);
  fake_val = argv[4][0] - '0';
  int ra_sel = argv[5][0] - '0';

  offset = 4*(range_offset + var_offset);
  int saved_offset = offset;
  void *ret_label = (void*)&main;
  void *fake_label = NULL;
  GET_LABEL_ADDRESS(ret_label,TARGET_LABEL);
  gvar_init(0);
  if(offset == -1) { GOTO_SAVED_LABEL(ret_label);}   // impossible to happen

  // call a function but illegally return
  fake_label = helper(ret_label,offset_sel,ra_sel);
  COMPILER_BARRIER;
  // the elligal return site
TARGET_LABEL(argc)
  // if hijack the return address, then retrun 0
  if(gvar() == 2) return 32+saved_offset/4;
  COMPILER_BARRIER;
  helper(fake_label,offset_sel,ra_sel);
  return gvar();
}
