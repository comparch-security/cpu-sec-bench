// assembly helper functions

#ifndef ASSEMBLY_HPP_INCLUDED
#define ASSEMBLY_HPP_INCLUDED

#include "include/gcc_builtin.hpp"

#define RA_POS_TEST        \
  RA_POS_TEST_ENTRY(0, 0)  \
  RA_POS_TEST_ENTRY(4, 1)  \
  RA_POS_TEST_ENTRY(8, 2)  \
  RA_POS_TEST_ENTRY(12, 3)  \
  RA_POS_TEST_ENTRY(16, 4)  \
  RA_POS_TEST_ENTRY(20, 5)  \
  RA_POS_TEST_ENTRY(24, 6)  \
  RA_POS_TEST_ENTRY(28, 7)  \
  RA_POS_TEST_ENTRY(32, 8)  \
  RA_POS_TEST_ENTRY(36, 9)  \
  RA_POS_TEST_ENTRY(40, 10)  \
  RA_POS_TEST_ENTRY(44, 11)  \
  RA_POS_TEST_ENTRY(48, 12)  \
  RA_POS_TEST_ENTRY(52, 13)  \
  RA_POS_TEST_ENTRY(56, 14)  \
  RA_POS_TEST_ENTRY(60, 15)  \

// detect ISA
#ifdef __x86_64
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

#if defined(__riscv) && __riscv_xlen == 64
  #define CSB_RV64GC
  typedef unsigned long long arch_uint_t;
  typedef long long arch_int_t;
  #include "riscv64/assembly.hpp"
#endif

// detect compiler
#if defined(__GNUC__)
  #if defined(__clang__)
    #define COMPILER_CLANG
  #else
    #define COMPILER_GCC
  #endif
#endif

extern void get_got_func(void **gotp, void *label, int cet);
extern void replace_got_func(void **fake, void *got);

#endif // ASSEMBLY_HPP_INCLUDED
