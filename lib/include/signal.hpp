#ifndef GCC_BUILTIN_SIGNAL_INCLUDED
#define GCC_BUILTIN_SIGNAL_INCLUDED

// information recorded for exception checking
struct sigact_record_t {
  union {
    void *faulty_addr; // faulty memory location: SIGILL, SIGFPE, SIGSEGV, SIGBUS, and SIGTRAP
  };
};

// catch exception for non-execution on writable pages
extern void begin_catch_nx_exception(void * expected_faulty_addr = (void *)0llu);
extern void   end_catch_nx_exception();

#endif
