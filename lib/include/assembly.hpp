// assembly helper functions

#ifndef ASSEMBLY_HPP_INCLUDED
#define ASSEMBLY_HPP_INCLUDED

#include "include/gcc_builtin.hpp"

// detect ISA
#if defined(__x86_64) || defined(_MSC_VER)
// strange that we cannot find a predefined macro to identify ISA in MSVC
// We are forced to check the compiler macro and assume Intel x86_64 accordingly
  #define CSB_X86_64
  typedef unsigned long long arch_uint_t;
  typedef long long arch_int_t;
  #include "x86_64/assembly.hpp"
#endif

#if defined(__ARM_ARCH) && __ARM_ARCH >= 8 && defined(__aarch64__)
  #define CSB_ARMV8_64
  typedef unsigned long long arch_uint_t;
  typedef long long arch_int_t;
  #include "aarch64/assembly.hpp"
#endif

#if defined(__riscv) && __riscv_xlen == 64 && !defined(__CHERI__ )
  #define CSB_RV64GC
  typedef unsigned long long arch_uint_t;
  typedef long long arch_int_t;
  #include "riscv64/assembly.hpp"
#endif

#if __CHERI_CAPABILITY_WIDTH__ == 128 && _riscv_arch == cheri  && defined(__CHERI__ )
  #define CSB_RV64GC
  typedef unsigned long long arch_uint_t;
  typedef long long arch_int_t;
  #include "cheri_riscv64/assembly.hpp"
#endif

// detect compiler
#if defined(__GNUC__)
  #if defined(__clang__)
    #define COMPILER_CLANG
  #else
    #define COMPILER_GCC
  #endif
#endif

#if defined(_MSC_VER)
  #define COMPILER_MSVC
#endif

extern void get_got_func(void **gotp, void *label, int cet);
extern void replace_got_func(void **fake, void *got);

#endif // ASSEMBLY_HPP_INCLUDED
