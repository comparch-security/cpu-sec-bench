#ifndef MSVC_BUILTIN_HPP_INCLUDED
#define MSVC_BUILTIN_HPP_INCLUDED

// make a function incline/non-inline
#define FORCE_INLINE __forceinline
#define FORCE_NOINLINE __declspec(noinline)

// a barrier to stop compiler from reorder memory operations
// For hardware access, use the /volatile:iso compiler option together with the volatile keyword.
#define COMPILER_BARRIER __faststorefence

// code/data alignment
#define ALIGN(NByte) __declspec ((aligned (NByte)))

#endif