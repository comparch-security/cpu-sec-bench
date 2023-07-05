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

        inline void begin_catch_exception(const void *expected_faulty_addr, 
        int si_code = 0, int rv_code = RT_CODE_ACCERR, int si_signo = SIGSEGV){

            switch (si_signo)
            {
            case SIGSEGV:
                switch (si_code)
                {
                case SEGV_MAPERR:
                    begin_catch_exception_msvc((PVOID*)expected_faulty_addr, 
                    (ULONG*)NULL, rv_code, (ULONG)EXCEPTION_IN_PAGE_ERROR);
                    break;
                case SEGV_ACCERR:
                    begin_catch_exception_msvc((PVOID*)expected_faulty_addr, 
                    (ULONG*)NULL, rv_code, (ULONG)EXCEPTION_ACCESS_VIOLATION);
                    break;
                default:
                    break;
                }
                break;
            case SIGILL:
                begin_catch_exception_msvc((PVOID*)expected_faulty_addr, 
                (ULONG*)NULL, rv_code, (ULONG)EXCEPTION_ILLEGAL_INSTRUCTION);
                break;
            case SIGFPE:
                begin_catch_exception_msvc((PVOID*)expected_faulty_addr, 
                (ULONG*)NULL, rv_code, (ULONG)EXCEPTION_FLT_INVALID_OPERATION);
                break;
            case SIGBUS:
                switch (si_code){
                    case BUS_ADRALN:
                        begin_catch_exception_msvc((PVOID*)expected_faulty_addr, 
                        (ULONG*)NULL, rv_code, (ULONG)EXCEPTION_DATATYPE_MISALIGNMENT);
                    break;
                }
            default:
                break;
            }

        }
        inline void begin_catch_exception(const void **expected_faulty_addr,
        int si_code = 0, int rv_code = RT_CODE_ACCERR, int si_signo = SIGSEGV){
            switch (si_signo)
            {
            case SIGSEGV:
                switch (si_code)
                {
                case SEGV_MAPERR:
                    begin_catch_exception_msvc((PVOID**)expected_faulty_addr, 
                    (ULONG*)NULL, rv_code, (ULONG)EXCEPTION_IN_PAGE_ERROR);
                    break;
                case SEGV_ACCERR:
                    begin_catch_exception_msvc((PVOID**)expected_faulty_addr, 
                    (ULONG*)NULL, rv_code, (ULONG)EXCEPTION_ACCESS_VIOLATION);
                    break;
                default:
                    break;
                }
                break;
            case SIGILL:
                begin_catch_exception_msvc((PVOID**)expected_faulty_addr, 
                (ULONG*)NULL, rv_code, (ULONG)EXCEPTION_ILLEGAL_INSTRUCTION);
                break;
            case SIGFPE:
                begin_catch_exception_msvc((PVOID**)expected_faulty_addr, 
                (ULONG*)NULL, rv_code, (ULONG)EXCEPTION_FLT_INVALID_OPERATION);
                break;
            case SIGBUS:
                switch (si_code){
                    case BUS_ADRALN:
                        begin_catch_exception_msvc((PVOID**)expected_faulty_addr, 
                        (ULONG*)NULL, rv_code, (ULONG)EXCEPTION_DATATYPE_MISALIGNMENT);
                    break;
                }
            default:
                break;
            }
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
