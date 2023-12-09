#ifndef GCC_BUILTIN_HPP_INCLUDED
#define GCC_BUILTIN_HPP_INCLUDED

// make a function incline/non-inline
#define FORCE_INLINE __attribute__((always_inline)) inline
#define FORCE_NOINLINE __attribute__((noinline))

// a barrier to stop compiler from reorder memory operations
#define COMPILER_BARRIER asm volatile("" : : : "memory")

// code/data alignment
#define ALIGN(NByte) __attribute__ ((aligned (NByte)))
GLOBAR_VAR_PRE int fake_use_arg;

#endif