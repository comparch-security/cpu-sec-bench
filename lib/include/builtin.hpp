// builtin macro functions

#ifndef BUILTIN_HPP_INCLUDED
#define BUILTIN_HPP_INCLUDED

    // detect compiler
    #if defined(_MSC_VER)
        #include "visualcpp/msvc_builtin.hpp"
        #define COMPILER_MSVC
    #endif

    #if defined(__GNUC__)
        #include "include/gcc_builtin.hpp"
        #if defined(__clang__)
            #define COMPILER_CLANG
        #else
            #define COMPILER_GCC
        #endif
    #endif

#endif