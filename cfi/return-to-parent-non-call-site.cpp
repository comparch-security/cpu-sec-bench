#include <cstdlib>
#include "include/global_var.hpp"
#include "include/result.hpp"
#include <string>

volatile arch_int_t offset;

void FORCE_NOINLINE helper(void *label) {
  gvar_init(3);
  COMPILER_BARRIER;
  GET_RAA_SP_OFFSET(offset);
  #ifdef TRACE_RUN
    ra_addr = offset;
    GET_RA_ADDR(ra_addr);
    WRITE_TRACE("RA address: 0x", ra_addr);
    WRITE_TRACE("RA before modified: 0x", *(long long*)ra_addr);
  #endif
  MOD_STACK_DAT(label, offset);
  /* HiFive Unmatched, GCC 11.2.0
   * Make sure offset is modified as otherwise
   * the stack is not expanded similarily with
   * the -within-analysis test.
   */
  offset = rand();
  gvar_init(0);
}

int main(int argc, char* argv[])
{
  INIT_TRACE_FILE;
  // get the offset of RA on stack
  // a literal -v token means the calibration variable was never recorded
  char *end = NULL;
  long offset_num = strtol(argv[1], &end, 10);
  if(end == argv[1] || *end != '\0') return 1;
  offset = 4 * offset_num;
  void *label = (void*)&main;
  GET_LABEL_ADDRESS(label, TARGET_LABEL);
  if(offset == -1) { GOTO_SAVED_LABEL(label);}   // impossible to happen

  gvar_init(2);
  // call a function but illegally return
  helper(label);
  gvar_init(4); // failed if runs here

  // the elligal return site
TARGET_LABEL(argc)
  if(0 == gvar()) exit(RT_CODE_SUCCESS);   // the hijacked RA reached this label
  exit(1);                                  // the RA was not hijacked
}
