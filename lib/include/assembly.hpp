// abi sensitive assembly helper functions

#ifndef ASSEMBLY_HPP_INCLUDED
#define ASSEMBLY_HPP_INCLUDED

#include "include/builtin.hpp"

// detect ISA
#if defined(__x86_64) || defined(_M_X64)
  #define CSB_X86_64
  typedef unsigned long long arch_uint_t;
  typedef long long arch_int_t;
  #define OP_ENDBR 0xfa1e0ff3
  #if defined(COMPILER_MSVC)
    #include "x86_64/visualcpp_assembly.hpp"
  #elif defined(COMPILER_GCC)
    #include "x86_64/assembly.hpp"
  #else
    #include "x86_64/assembly.hpp"
  #endif
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

#if defined(__riscv) && __CHERI_CAPABILITY_WIDTH__ == 128 && _riscv_arch == cheri  && defined(__CHERI__ )
  #define CSB_RV64GC
  typedef unsigned long long arch_uint_t;
  typedef long long arch_int_t;
  #include "cheri_riscv64/assembly.hpp"
#endif

extern void get_got_func(void **gotp, void *label, int cet);
extern void replace_got_func(void **fake, void *got);

// The return address of the current frame.  Keyed on the compiler rather than the
// ISA: every GCC/clang port reads it with one builtin, and only MSVC spells it
// differently.  A callee needs the value -- not an offset -- to locate its own
// saved RA slot, because the slot's offset follows this frame's shape.
#if defined(COMPILER_MSVC)
  #define GET_RETURN_ADDRESS(v) ((v) = (void *)_ReturnAddress())
#else
  #define GET_RETURN_ADDRESS(v) ((v) = __builtin_return_address(0))
#endif

#endif // ASSEMBLY_HPP_INCLUDED
