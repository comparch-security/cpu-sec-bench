#include "include/global_var.hpp"
#include "include/assembly.hpp"

// Todo: we need to optimize this code to get the frame size for different
// return type, argument type and stack type.

void FORCE_NOINLINE helper_v_v() {
  // Here the loc is optimized as a register which does not occupy space on stack
  // but $rbx is pushed on stack on x86_64 due to the later function call.
  // The function is therefore named as void *fp(void) with no internal storage.
  arch_int_t loc;

  #ifdef CSB_X86_64
  asm volatile("mov %%rsp, %0;" : "=r"(loc));
  #endif

  gvar_init(gvar() - loc);
}

int main(int argc, char* argv[])
{
  arch_int_t loc;

  #ifdef CSB_X86_64
  asm volatile("mov %%rsp, %0;" : "=r"(loc));
  #endif

  gvar_init(loc);

  helper_v_v();

  if(gvar() > 0) return gvar()/4 + 32;
  else return 1;
}
