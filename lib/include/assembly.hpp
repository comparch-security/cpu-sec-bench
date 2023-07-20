// abi sensitive assembly helper functions

#ifndef ASSEMBLY_HPP_INCLUDED
#define ASSEMBLY_HPP_INCLUDED

#include "include/builtin.hpp"

// detect ISA
#if defined(__x86_64) || defined(_M_X64)
  #define CSB_X86_64
  typedef unsigned long long arch_uint_t;
  typedef long long arch_int_t;
#endif

#if defined(COMPILER_MSVC)
  #include <Windows.h>
  #include "x86_64/visualcpp_assembly.hpp"
#elif defined(COMPILER_GCC)
  #include "x86_64/assembly.hpp"
#else
  #include "x86_64/assembly.hpp"
#endif

#if defined(__ARM_ARCH) && __ARM_ARCH >= 8 && defined(__aarch64__)
  #define CSB_ARMV8_64
  typedef unsigned long long arch_uint_t;
  typedef long long arch_int_t;
  #include "aarch64/assembly.hpp"
#endif

#if defined(__riscv) && __riscv_xlen == 64
  #define CSB_RV64GC
  typedef unsigned long long arch_uint_t;
  typedef long long arch_int_t;
  #include "riscv64/assembly.hpp"
#endif

extern void get_got_func(void **gotp, void *label, int cet);
extern void replace_got_func(void **fake, void *got);

#endif // ASSEMBLY_HPP_INCLUDED
