#ifndef GCC_BUILTIN_SIGNAL_INCLUDED
#define GCC_BUILTIN_SIGNAL_INCLUDED

const int RT_CODE_MISMATCH  = -1;      // ERROR: mismatched signal
const int RT_CODE_MAPERR    = 15;      // address not accessible
const int RT_CODE_ACCERR    = 16;      // no permission to access

// catch exception for non-execution on writable pages
extern void begin_catch_exception_gcc(const void *expected_faulty_addr, int si_code = 0, int rv_code = RT_CODE_ACCERR, int si_signo = SIGSEGV);
extern void begin_catch_exception_gcc(const void **expected_faulty_addr, int si_code = 0, int rv_code = RT_CODE_ACCERR, int si_signo = SIGSEGV);
extern void   end_catch_exception();

#endif
