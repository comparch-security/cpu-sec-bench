#include <cstdlib>
#include "include/global_var.hpp"
#include "include/signal.hpp"

static unsigned char m_data[] = FUNC_MACHINE_CODE;
const unsigned char m_rodata[] = FUNC_MACHINE_CODE;
typedef void (*func_t)();

volatile arch_int_t offset;

void FORCE_NOINLINE return_helper(void *p) {
  gvar_init(2);
  GET_RAA_SP_OFFSET(offset);
  #ifdef TRACE_RUN
    ra_addr = offset;
    GET_RA_ADDR(ra_addr);
    WRITE_TRACE("RA address: 0x", ra_addr);
    WRITE_TRACE("RA before modified: 0x", *(long long*)ra_addr);
  #endif
  MOD_STACK_DAT(p, offset);
  WRITE_TRACE("RA address: 0x", ra_addr);
  WRITE_TRACE("RA after modified: 0x", *(long long*)ra_addr);
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
  WRITE_TRACE("Successful Jumped", "");
}

int main(int argc, char* argv[])
{
  INIT_TRACE_FILE;
  gvar_init(1);
  unsigned char m_stack[] = FUNC_MACHINE_CODE;
  unsigned char *m_heap = new unsigned char [16];
  assign_fake_machine_code(m_heap);

  func_t f = NULL;
  void *p = NULL;
  void *l = NULL;
  char *addr = NULL;

  void* bypass_pos = (void*)&main;
  void* exit_pos   = (void*)&main;
  GET_LABEL_ADDRESS(bypass_pos,TARGET_LABEL);
  GET_LABEL_ADDRESS(exit_pos,TARGET_LABEL1);

  if(argc < 3) return 1; // malformed arguments: the injection was never staged
  switch(argv[1][0] - '0') {
  case 0: // return
    if(argc < 4) return 1;
    switch(argv[2][0] - '0') {
    case 0: p = (void *)(m_rodata); break;
    case 1: p = (void *)(m_data); break;
    case 2: p = (void *)(m_stack); break;
    case 3: p = (void *)(m_heap); break;
    default: return 1;
    }
    {
      char *end = NULL;
      long offset_num = strtol(argv[3], &end, 10);
      // a literal -v token means the calibration variable was never recorded
      if(end == argv[3] || *end != '\0') return 1;
      offset = 4 * offset_num;
    }
    addr = (char *)p;
    break;
  case 1: // call
    switch(argv[2][0] - '0') {
    case 0: f = (func_t)(void*)(m_rodata); break;
    case 1: f = (func_t)(void*)(m_data); break;
    case 2: f = (func_t)(void*)(m_stack); break;
    case 3: f = (func_t)(void*)(m_heap); break;
    default: return 1;
    }
    addr = (char *)f;
    break;
  case 2: // jump
    if(argc < 4) return 1;
    switch(argv[2][0] - '0') {
    case 0:
      l = argv[3][0] - '0' ? bypass_pos : exit_pos; // neither of them is used
      SET_MEM(&l, m_rodata); break;
    case 1:
      l = argv[3][0] - '0' ? bypass_pos : exit_pos; // neither of them is used
      SET_MEM(&l, m_data); break;
    case 2:
      l = argv[3][0] - '0' ? bypass_pos : exit_pos; // neither of them is used
      SET_MEM(&l, m_stack); break;
    case 3:
      l = argv[3][0] - '0' ? bypass_pos : exit_pos; // neither of them is used
      SET_MEM(&l, m_heap); break;
    default: return 1;
    }
    addr = (char *)l;
    break;
  default:
    return 1;
  }
  #if defined(COMPILER_MSVC)
    // msvc_code_conv() maps only MAPERR/ACCERR; the kernel's MTE predicate has no
    // Windows counterpart, so the registration keeps the access-error predicate there
    begin_catch_exception(addr, SEGV_ACCERR, RT_CODE_MTE_TAGCHECK, SIGSEGV);
  #else
    begin_catch_exception(addr, RT_SICODE_MTESERR, RT_CODE_MTE_TAGCHECK, SIGSEGV);//aarch64 mte hwasan returns 99
  #endif
  begin_catch_exception(addr, SEGV_ACCERR);
#ifdef CSB_ARMV8_64
  // bus error on Apple M1
  begin_catch_exception(addr, BUS_ADRALN, RT_CODE_ACCERR, SIGBUS);
#endif
#ifdef COMPILER_MSVC
  begin_catch_exception(addr+4, 0, RT_CODE_SUCCESS, SIGSEGV);
#endif
  begin_catch_exception(addr+4, 0, RT_CODE_SUCCESS, SIGILL);
  begin_catch_exception(addr+4, 0, RT_CODE_SUCCESS, SIGFPE);
  switch(argv[1][0] - '0') {
  case 0: return_helper(p); break;
  case 1: call_helper(f); break;
  case 2: { GOTO_SAVED_LABEL(l);WRITE_TRACE("Successful Jumped", "");}
  }
TARGET_LABEL(argc)
  end_catch_exception();
  end_catch_exception();
#ifdef CSB_ARMV8_64
  end_catch_exception();
#endif
#ifdef COMPILER_MSVC
  end_catch_exception();
#endif
  end_catch_exception();
  end_catch_exception();
TARGET_LABEL1(argc)
  delete [] m_heap;
  return 1; // no hijack landed: the injected code was never executed
}
