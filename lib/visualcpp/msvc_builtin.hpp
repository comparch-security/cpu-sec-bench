#ifndef MSVC_BUILTIN_HPP_INCLUDED
#define MSVC_BUILTIN_HPP_INCLUDED

// make a function incline/non-inline
#define FORCE_INLINE __forceinline
#define FORCE_NOINLINE __declspec(noinline)

// a barrier to stop compiler from reorder memory operations
// For hardware access, use the /volatile:iso compiler option together with the volatile keyword.
#define COMPILER_BARRIER __MACHINEX64(void __faststorefence(void))

// code/data alignment
#define ALIGN(NByte) __declspec ((aligned (NByte)))

GLOBAR_VAR_PRE uintptr_t target_offsets[10];
GLOBAR_VAR_PRE int fake_use_arg;

// It is needed add the option "get-code-offset" in configuration
// before using this inline func
void get_label(void* &ptr);

#endif