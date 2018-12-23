#ifndef GCC_BUILTIN_SIGNAL_INCLUDED
#define GCC_BUILTIN_SIGNAL_INCLUDED

// information recorded for exception checking
struct sigact_record_t {
  union {
    // faulty memory location: SIGILL, SIGFPE, SIGSEGV, SIGBUS, and SIGTRAP
    const void *faulty_data_addr;
    const void **faulty_func_addr;
  };
};

// catch exception for non-execution on writable pages
extern void begin_catch_nx_exception(const void *expected_faulty_addr);
extern void begin_catch_nx_exception(const void **expected_faulty_addr);
extern void begin_catch_nx_exception();
extern void   end_catch_nx_exception();

#endif
