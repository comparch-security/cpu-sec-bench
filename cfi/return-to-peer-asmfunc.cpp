#include <cstdlib>
#include "include/global_var.hpp"
#include <string>

volatile arch_int_t offset = 0;

void FORCE_NOINLINE helper(void * label) {

  /* The saved return address sits at an offset that follows this frame's shape,
   * so the offset calibrated for another function's frame (stack-offset-v-p-g1)
   * does not aim at it: on riscv64 that write lands on the caller's canary slot
   * and the stack protector aborts the process.  Locate the slot by the value it
   * holds -- this frame's own return address -- and write nothing when it is not
   * found, leaving the caller's not-landed path to deliver 1.
   */
  void *ra_val = NULL;
  GET_RETURN_ADDRESS(ra_val);
  void *ra_stack = NULL;
  arch_int_t ra_offset = -1;
  #define RA_POS_TEST_ENTRY(off, rv)                    \
    READ_STACK_DAT_IMM(ra_stack, off);                  \
    if(ra_offset < 0 && ra_stack == ra_val) ra_offset = off;
  RA_POS_TEST
  #undef RA_POS_TEST_ENTRY
  #ifdef TRACE_RUN
    if(ra_offset >= 0) {
      ra_addr = ra_offset;
      GET_RA_ADDR(ra_addr);
      WRITE_TRACE("RA address: 0x", ra_addr);
      WRITE_TRACE("RA before modified: 0x", *(long long*)ra_addr);
    }
  #endif
  if(ra_offset >= 0) {
    MOD_STACK_DAT(label, ra_offset);
    WRITE_TRACE("RA address: 0x", ra_addr);
    WRITE_TRACE("RA after modified: 0x", *(long long*)ra_addr);
  }
  /* HiFive Unmatched, GCC 11.2.0
   * Make sure the frame is expanded and the return address spilled: without a
   * call in this function the helper would be a leaf and save no RA slot.
   */
  offset = rand();
}

int main(int argc, char* argv[])
{
  INIT_TRACE_FILE;
  // the peer assembly return site is reached with a fixed state value
  gvar_init(2);
  void *label = (void *)(assembly_return_site);
  helper(label);
  gvar_decr();
  return 1; // the RA was not hijacked to the peer assembly return site
}