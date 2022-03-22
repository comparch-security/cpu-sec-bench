#ifndef CSB_GLOBAL_VAR_HPP
#define CSB_GLOBAL_VAR_HPP

#include "include/assembly.hpp"

extern void gvar_init(arch_int_t);
extern void gvar_incr();
extern void gvar_decr();
extern void gvar_add(arch_int_t);
extern arch_int_t gvar();

#endif
