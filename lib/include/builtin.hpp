// builtin macro functions

#ifndef BUILTIN_HPP_INCLUDED
#define BUILTIN_HPP_INCLUDED

#if defined(INTERN_RELATED_GLOBALVAR)
//header file with the definition of globar var in source file
#define GLOBAR_VAR_PRE
#elif defined(EXTERN_RELATED_GLOBALVAR)
//header file without the definition of globar var in source file
#define GLOBAR_VAR_PRE extern
#else
#define GLOBAR_VAR_PRE extern
#endif

#if defined(TRACE_INTERN_RELATED_GLOBALVAR)
//header file with the definition of globar var in source file
#define TRACE_GLOBAR_VAR_PRE
#elif defined(TRACE_EXTERN_RELATED_GLOBALVAR)
//header file without the definition of globar var in source file
#define TRACE_GLOBAR_VAR_PRE extern
#else
#define TRACE_GLOBAR_VAR_PRE
#endif

TRACE_GLOBAR_VAR_PRE long long ra_addr;

#if defined(DEBUG_OUTPUT) || defined(TRACE_RUN)
#include <iostream>
#endif

#ifdef TRACE_RUN
  #include <fstream>
  #include <string>
   TRACE_GLOBAR_VAR_PRE std::string trace_file_name;
   TRACE_GLOBAR_VAR_PRE std::ofstream trace_file;
  #define INIT_TRACE_FILE  trace_file_name = argv[0];              \
            size_t ind = trace_file_name.find('/');                   \
            if(ind != std::string::npos){                    \
                trace_file_name = trace_file_name.substr(ind+1);         \
            }                                                \
            trace_file_name = LOG_PATH + trace_file_name + ".tracelog";

  #define WRITE_TRACE(prompt,ptr)                                                     \
                trace_file.open (trace_file_name, std::ofstream::out | std::ofstream::app); \
                if(!trace_file.fail()) {                                              \
                  trace_file << prompt << std::hex << ptr;                            \
                }else{                                                                \
                  std::cout << "Open trace log file failed" << std::endl;             \
                }                                                                     \
                trace_file.close()

#else

  #define INIT_TRACE_FILE {}
  #define WRITE_TRACE {}

#endif

// detect compiler
#if defined(_MSC_VER)
  #include <Windows.h>
  #include<intrin.h>
  #include <fstream>
  #include "visualcpp/msvc_builtin.hpp"
  #define COMPILER_MSVC
  #define PATH_SEPARATOR "\\"
  // It is needed add the option "get-code-offset" in configuration
  // before using this inline func
  FORCE_NOINLINE void labelfunc();
  #define GET_LABEL_ADDRESS(p,label_case)   \
    get_label(p)
  #define GOTO_SAVED_LABEL(p)               \
    do{                                     \
      jum_target.func_num = (long long) p;  \
      jum_target.func_ptr();                \
    }while(0)
  #define TARGET_LABEL(fake_var)            \
    labelfunc();
  #define TARGET_LABEL1(fake_var)           \
    labelfunc();
  #define DllImport   __declspec( dllimport )
  #define DllExport   __declspec( dllexport )
  inline long int GET_PAGE_SIZE(){
      SYSTEM_INFO si;
      GetSystemInfo(&si);
      return (long int) si.dwPageSize;
  }
  #define LOG_PATH ".\\trace-log\\"
#endif

#if defined(__GNUC__) || !defined(_MSC_VER)
  #if defined(TRACE_RUN)
    #include <fstream>
  #endif
  #include <unistd.h>
  #include "posix/gcc_builtin.hpp"
  #if defined(__clang__)
    #define COMPILER_CLANG
  #else
    #define COMPILER_GCC
  #endif
  #define PATH_SEPARATOR "/"
  #define GET_LABEL_ADDRESS(p,label_case)\
    p = &&label_case;
  #define GOTO_SAVED_LABEL(p)            \
    goto *p;
  #define TARGET_LABEL(fake_var)         \
    TARGET_LABEL:
  #define TARGET_LABEL_SUPRESS_OPT(fake_var) \
    TARGET_LABEL:
  #define TARGET_LABEL1(fake_var)        \
    TARGET_LABEL1:
  #define DllImport   
  #define DllExport   
  #define GET_RAA_SP_OFFSET(offset)
  inline long int GET_PAGE_SIZE(){
    return sysconf(_SC_PAGESIZE);
  }
  #define LOG_PATH "./trace-log/"
#endif

#endif
