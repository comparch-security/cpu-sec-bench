// assembly helper functions

#ifndef ASSEMBLY_HPP_INCLUDED
#define ASSEMBLY_HPP_INCLUDED

// make a function incline/non-inline
#define FORCE_INLINE __attribute__((always_inline)) inline
#define FORCE_NOINLINE __attribute__((noinline))

#ifdef ARCH_X86_64
  #include "lib/x86_64/assembly.hpp"
#endif

#endif // ASSEMBLY_HPP_INCLUDED

 
