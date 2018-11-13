// assembly helper functions

#ifndef ASSEMBLY_HPP_INCLUDED
#define ASSEMBLY_HPP_INCLUDED

// a barrier to stop compiler from reorder memory operations
#define COMPILER_BARRIER asm volatile("" : : : "memory")

#ifdef ARCH_X86_64
  #include "x86_64/assembly.hpp"
#endif

#endif // ASSEMBLY_HPP_INCLUDED
