// builtin macro functions

#ifndef BUILTIN_HPP_INCLUDED
#define BUILTIN_HPP_INCLUDED

#ifdef OTHER_OBJECT_GLOBARVAR
//header file with the definition of globar var in source file
#define GLOBAR_VAR_PRE
#else
//header file without the definition of globar var in source file
#define GLOBAR_VAR_PRE extern
#endif

#ifdef DEBUG_OUTPUT
#include <iostream>
#endif

// detect compiler
#if defined(_MSC_VER)
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
#endif

#if defined(__GNUC__)
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
#endif

#endif
