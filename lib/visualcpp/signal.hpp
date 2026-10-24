#ifndef VISUALCPP_BUILTIN_SEH_INCLUDED
#define VISUALCPP_BUILTIN_SEH_INCLUDED

#include<Windows.h>
#include<winnt.h>
#include<Excpt.h>

#include "include/result.hpp"

// catch exception for non-execution on writable pages
extern void begin_catch_exception_msvc(const PVOID expected_faulty_addr, ULONG* si_code = NULL, int rv_code = RT_CODE_ACCERR, DWORD si_signo = EXCEPTION_ACCESS_VIOLATION);
extern void begin_catch_exception_msvc(const PVOID* expected_faulty_addr, ULONG* si_code = NULL, int rv_code = RT_CODE_ACCERR, DWORD si_signo = EXCEPTION_ACCESS_VIOLATION);
extern void   end_catch_exception();

#endif