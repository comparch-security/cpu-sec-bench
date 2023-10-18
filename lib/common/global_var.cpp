#define TRACE_EXTERN_RELATED_GLOBALVAR
#include "include/global_var.hpp"

volatile static arch_int_t g = 0;

void gvar_init(arch_int_t a) { g = a; }

void gvar_incr() { g++; }

void gvar_decr() { g--; }

void gvar_add(arch_int_t a) {g += a; }

arch_int_t gvar() { return g; }


