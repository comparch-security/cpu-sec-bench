// api sensitive signal functions

#ifndef SIGNAL_HPP_INCLUDED
#define SIGNAL_HPP_INCLUDED

    #include <csignal>
    #include <cstdlib>
    #include <cstdio>
    #include <vector>
    #include "include/builtin.hpp"
    // detect compiler
    #if defined(_MSC_VER)
        /*replace begin_catch_exception() with
        begin_catch_exception_msvc() to smooth the gap between diff
        CAPI*/
        #include "visualcpp/signal.hpp"
        //define posix sig num for visualc++ porting
        #define SIGBUS 20
        //define posix sig code for visualc++ porting
        //so the macro value is meanless
        #define SEGV_MAPERR 1
        #define SEGV_ACCERR 2
        #define BUS_ADRALN  3

        inline ULONG msvc_code_conv(const int si_code, const int si_signo) {
          switch (si_signo) {
          case SIGSEGV:
            switch (si_code) {
            case SEGV_MAPERR: return EXCEPTION_IN_PAGE_ERROR;
            case SEGV_ACCERR: return EXCEPTION_ACCESS_VIOLATION;
            default:          break;
            }
          case SIGILL: return EXCEPTION_ILLEGAL_INSTRUCTION;
          case SIGFPE: return EXCEPTION_FLT_INVALID_OPERATION;
          case SIGBUS: return EXCEPTION_DATATYPE_MISALIGNMENT;
          default: break;
          }

          return 0; // should not run here?
        }

        inline void begin_catch_exception(const void *expected_faulty_addr, 
                                          int si_code = 0, int rv_code = RT_CODE_ACCERR, int si_signo = SIGSEGV) {
          begin_catch_exception_msvc((PVOID*)expected_faulty_addr, 
                                     (ULONG*)NULL, rv_code, msvc_code_conv(si_code, si_signo));
        }

        inline void begin_catch_exception(const void **expected_faulty_addr,
                                          int si_code = 0, int rv_code = RT_CODE_ACCERR, int si_signo = SIGSEGV) {
          begin_catch_exception_msvc((PVOID**)expected_faulty_addr, 
                                     (ULONG*)NULL, rv_code, msvc_code_conv(si_code, si_signo));
        }

    #endif

    #if defined(__GNUC__)
        /*for posix, just pass it to original begin_catch_exception() func*/
        #include "posix/signal.hpp"
        inline void begin_catch_exception(const void *expected_faulty_addr, 
        int si_code = 0, int rv_code = RT_CODE_ACCERR, int si_signo = SIGSEGV){
            begin_catch_exception_gcc(expected_faulty_addr,si_code,rv_code,si_signo);
        }
        inline void begin_catch_exception(const void **expected_faulty_addr, 
        int si_code = 0, int rv_code = RT_CODE_ACCERR, int si_signo = SIGSEGV){
            begin_catch_exception_gcc(expected_faulty_addr,si_code,rv_code,si_signo);
        }
    #endif

#endif
