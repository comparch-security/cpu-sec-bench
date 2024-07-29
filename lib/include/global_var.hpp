#ifndef CSB_GLOBAL_VAR_HPP
#define CSB_GLOBAL_VAR_HPP

#include "include/assembly.hpp"

extern "C" void gvar_init(arch_int_t);
extern "C" void gvar_incr();
extern "C" void gvar_decr();
extern "C" void gvar_add(arch_int_t);
extern "C" arch_int_t gvar();

#endif
