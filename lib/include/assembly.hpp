// assembly helper functions

#ifndef ASSEMBLY_HPP_INCLUDED
#define ASSEMBLY_HPP_INCLUDED

#include "include/gcc_builtin.hpp"

// a barrier to stop compiler from reorder memory operations
#define COMPILER_BARRIER asm volatile("" : : : "memory")

#ifdef __x86_64
  #include "x86_64/assembly.hpp"
#endif

#ifdef __riscv
  #if __riscv_xlen == 64
    #include "riscv64/assembly.hpp"
  #endif
#endif

#endif // ASSEMBLY_HPP_INCLUDED
