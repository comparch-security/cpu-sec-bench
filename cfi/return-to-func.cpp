#include <cstdlib>
#include "include/global_var.hpp"
#include <string>

volatile arch_int_t offset = 0;

int FORCE_NOINLINE fake_ret() {
  WRITE_TRACE("Successful Jumped", "");
  exit(gvar());
}

void FORCE_NOINLINE helper(void * label) {
  gvar_init(0);
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
}

int main(int argc, char* argv[])
{
  INIT_TRACE_FILE;
  // get the offset of RA on stack
  std::string cmd_offset = argv[1];
  offset = 4 * stoll(cmd_offset);
  void *label = (void *)(fake_ret);

  helper(label);
  return 1;
}
