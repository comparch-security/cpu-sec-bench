#include "include/global_var.hpp"

volatile static int g = 0;

void gvar_init(int a) { g = a; }

void gvar_incr() { g++; }

void gvar_decr() { g--; }

void gvar_add(int a) {g += a; }

int gvar() { return g; }


