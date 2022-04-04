#include <cstdlib>
#include "include/global_var.hpp"
#include "include/signal.hpp"

static unsigned char m_data[] = FUNC_MACHINE_CODE;
const unsigned char m_rodata[] = FUNC_MACHINE_CODE;
typedef void (*func_t)();

volatile arch_int_t offset;

void FORCE_NOINLINE return_helper(void *p) {
  gvar_init(2);
  MOD_STACK_DAT(p, offset);
  /* HiFive Unmatched, GCC 11.2.0
   * Make sure offset is modified as otherwise
   * the stack is not expanded similarily with
   * the -within-analysis test.
   */
  offset = rand();
}

void FORCE_NOINLINE call_helper(func_t f) {
  gvar_init(2);
  COMPILER_BARRIER;
  f();
}

int main(int argc, char* argv[])
{
  gvar_init(1);
  unsigned char m_stack[] = FUNC_MACHINE_CODE;
  unsigned char *m_heap = new unsigned char [16];
  assign_fake_machine_code(m_heap);

  func_t f = NULL;
  void *p = NULL;
  void *l = NULL;
  char *addr = NULL;

  switch(argv[1][0] - '0') {
  case 0: // return
    switch(argv[2][0] - '0') {
    case 0: p = (void *)(m_rodata); break;
    case 1: p = (void *)(m_data); break;
    case 2: p = (void *)(m_stack); break;
    case 3: p = (void *)(m_heap); break;
    }
    offset = 4 * (argv[3][0] - '0');
    addr = (char *)p;
    break;
  case 1: // call
    switch(argv[2][0] - '0') {
    case 0: f = (func_t)(m_rodata); break;
    case 1: f = (func_t)(m_data); break;
    case 2: f = (func_t)(m_stack); break;
    case 3: f = (func_t)(m_heap); break;
    }
    addr = (char *)f;
    break;
  case 2: // jump
    switch(argv[2][0] - '0') {
    case 0:
      l = argv[3][0] - '0' ? &&BYPASS_POS : &&EXIT_POS; // neither of them is used
      SET_MEM(&l, m_rodata); break;
    case 1:
      l = argv[3][0] - '0' ? &&BYPASS_POS : &&EXIT_POS; // neither of them is used
      SET_MEM(&l, m_data); break;
    case 2:
      l = argv[3][0] - '0' ? &&BYPASS_POS : &&EXIT_POS; // neither of them is used
      SET_MEM(&l, m_stack); break;
    case 3:
      l = argv[3][0] - '0' ? &&BYPASS_POS : &&EXIT_POS; // neither of them is used
      SET_MEM(&l, m_heap); break;
    }
    addr = (char *)l;
    break;
  }

  begin_catch_exception(addr, SEGV_ACCERR);
#ifdef CSB_ARMV8_64
  // bus error on Apple M1
  begin_catch_exception(addr, BUS_ADRALN, RT_CODE_ACCERR, SIGBUS);
#endif
  begin_catch_exception(addr+4, 0, 0, SIGILL);
  begin_catch_exception(addr+4, 0, 0, SIGFPE);
  switch(argv[1][0] - '0') {
  case 0: return_helper(p); break;
  case 1: call_helper(f); break;
  case 2: goto *l;
  }
 BYPASS_POS:
  end_catch_exception();
  end_catch_exception();
#ifdef CSB_ARMV8_64
  end_catch_exception();
#endif
  end_catch_exception();
 EXIT_POS:
  return gvar();
}
