// assembly helper functions

#ifndef ASSEMBLY_HPP_INCLUDED
#define ASSEMBLY_HPP_INCLUDED

// make a function incline/non-inline
#define FORCE_INLINE __attribute__((always_inline)) inline
#define FORCE_NOINLINE __attribute__((noinline))

// a barrier to stop compiler from reorder memory operations
#define COMPILER_BARRIER asm volatile("" : : : "memory")

#ifdef ARCH_X86_64
  #include "lib/x86_64/assembly.hpp"
#endif

#endif // ASSEMBLY_HPP_INCLUDED

 
