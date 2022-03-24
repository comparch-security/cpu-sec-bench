// assembly helper functions

#ifndef ASSEMBLY_HPP_INCLUDED
#define ASSEMBLY_HPP_INCLUDED

#include "include/gcc_builtin.hpp"

// a barrier to stop compiler from reorder memory operations
#define COMPILER_BARRIER asm volatile("" : : : "memory")

// non-leaf function enforcement
extern int dummy_leaf_rv;
extern int dummy_leaf_func(int);
#define ENFORCE_NON_LEAF_FUNC_VAR(VAR) dummy_leaf_rv = dummy_leaf_func(VAR);
#define ENFORCE_NON_LEAF_FUNC dummy_leaf_rv = dummy_leaf_func(dummy_leaf_rv);

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

extern void get_got_func(void **gotp, void *label, int cet);
extern void replace_got_func(void **fake, void *got);

#endif // ASSEMBLY_HPP_INCLUDED
