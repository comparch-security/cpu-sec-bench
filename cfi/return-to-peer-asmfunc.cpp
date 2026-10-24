#include <cstdlib>
#include "include/global_var.hpp"
#include <string>

volatile arch_int_t offset = 0;

void FORCE_NOINLINE helper(void * label) {

  GET_RAA_SP_OFFSET(offset);
  #ifdef TRACE_RUN
    GET_SP_BASE(ra_addr);
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
  // get the offset of RA on stack
  // a literal -v token means the calibration variable was never recorded
  char *end = NULL;
  long offset_num = strtol(argv[1], &end, 10);
  if(end == argv[1] || *end != '\0') return 1;
  // fake compiler: the peer assembly return site is reached with a fixed state value
  offset = 4 * offset_num;
  gvar_init(2);
  void *label = (void *)(assembly_return_site);
  helper(label);
  gvar_decr();
  return 1; // the RA was not hijacked to the peer assembly return site
}