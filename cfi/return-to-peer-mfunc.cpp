#include <cstdlib>
#include <cstring>
#include "include/cfi.hpp"
#include <string>

volatile arch_int_t offset = 0;

class target_obj {
    int id;
public:
    target_obj(int id): id(0) {}
    void fake_ret() {
        WRITE_TRACE("Successful Jumped", "");
        exit(gvar());
    }
};

typedef void (target_obj::*MemberFuncPtr)(void);

void FORCE_NOINLINE helper(void* label) {
  gvar_init(0);
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
  target_obj fake_obj(0);
  MemberFuncPtr label = &target_obj::fake_ret;
  void* ptr = NULL;
  memcpy(&ptr, &label, sizeof(void*));
  helper(ptr);
  return 1;
}
