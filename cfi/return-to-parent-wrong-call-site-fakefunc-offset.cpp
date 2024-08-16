#include <cstdlib>
#include "include/global_var.hpp"
#include <string>
#include <iostream>

volatile arch_int_t offset;
int fake_init_val; // init in configure.json and is -2
void FORCE_NOINLINE helper(void *label) {
  COMPILER_BARRIER;
  GET_RAA_SP_OFFSET(offset);
  #ifdef TRACE_RUN
    GET_SP_BASE(ra_addr);
    WRITE_TRACE("RA address: 0x", ra_addr);
    WRITE_TRACE("RA before modified: 0x", *(long long*)ra_addr);
  #endif
  if(gvar() < 0){
    MOD_STACK_DAT(label, offset);
  }
  WRITE_TRACE("RA address: 0x", ra_addr);
  WRITE_TRACE("RA after modified: 0x", *(long long*)ra_addr);
  /* HiFive Unmatched, GCC 11.2.0
   * Make sure offset is modified as otherwise
   * the stack is not expanded similarily with
   * the -within-analysis test.
   */
  offset = rand();
}

void* FORCE_NOINLINE helper2(int val) {
  void* curr_return_addr = NULL;
  GET_RA_ADDR(curr_return_addr);
  if(fake_init_val > 0) gvar_init(fake_init_val); //imposibel happen
  return curr_return_addr;
}

int main(int argc, char* argv[])
{
  INIT_TRACE_FILE;
  // get the offset of RA on stack
  if(argc == 3){
    std::string cmd_offset = argv[1];
    offset = 4 * stoll(cmd_offset);
    fake_init_val = stoll(std::string(argv[2]));
  }else{
    std::string cmd_offset = argv[1];
    std::string extra_offset = argv[2];
    offset = 4*(stoll(cmd_offset) + stoll(extra_offset));
    fake_init_val = stoll(std::string(argv[3]));
  }
  void *ret_label = (void*)&main;
  gvar_init(fake_init_val);
  // call a function but illegally return
  ret_label = helper2(fake_init_val);
  gvar_incr();
  helper(ret_label); // if successful, it will return to gvar_incr()
  COMPILER_BARRIER;

  exit(gvar());// if failed, it will return to -1
}
