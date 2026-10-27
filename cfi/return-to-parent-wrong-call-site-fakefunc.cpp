#include <cstdlib>
#include "include/global_var.hpp"
#include "include/signal.hpp"
#include <string>

volatile arch_int_t offset;
int fake_init_val; // init in configure.json and is -2
void FORCE_NOINLINE helper(void *label) {
  COMPILER_BARRIER;
  GET_RAA_SP_OFFSET(offset);
  #ifdef TRACE_RUN
    ra_addr = offset;
    GET_RA_ADDR(ra_addr);
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
   * the return-to-parent-same-call-site test.
   */
  offset = rand();
}

FORCE_NOINLINE void* helper2(int val) {
  void* curr_return_addr = NULL;
  READ_STACK_DAT(curr_return_addr, offset);
  if(fake_init_val > 0) gvar_init(fake_init_val); //impossible to happen
  return curr_return_addr;
}

int main(int argc, char* argv[])
{
  INIT_TRACE_FILE;
  // get the offset of RA on stack
  if(argc < 3) return 1; // malformed arguments: the corruption was never staged
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
  // Contain a fault raised from the corrupted frame: a hijacked return can land
  // anywhere and fault as any signal, so the registration is address- and
  // code-wildcard across all four signos.  helper2()'s own stack reads run above,
  // outside the window.
  begin_catch_exception((void *)NULL, 0, RT_CODE_FAULT_CONTAINED, SIGSEGV);
  begin_catch_exception((void *)NULL, 0, RT_CODE_FAULT_CONTAINED, SIGILL);
  begin_catch_exception((void *)NULL, 0, RT_CODE_FAULT_CONTAINED, SIGFPE);
  begin_catch_exception((void *)NULL, 0, RT_CODE_FAULT_CONTAINED, SIGBUS);
  helper(ret_label); // if successful, it will return to gvar_incr()
  end_catch_exception();
  end_catch_exception();
  end_catch_exception();
  end_catch_exception();
  COMPILER_BARRIER;

  if(0 == gvar()) exit(RT_CODE_SUCCESS); // the hijacked return reached gvar_incr()
  exit(1);                                // the designed failure: the hijack did not take
}