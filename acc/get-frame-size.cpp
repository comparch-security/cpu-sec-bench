#include "include/global_var.hpp"
#include "include/assembly.hpp"
#include "include/temp_file.hpp"

// Todo: we need to optimize this code to get the frame size for different
// return type, argument type and stack type.

void FORCE_NOINLINE helper_v_v() {
  // Here the loc is optimized as a register which does not occupy space on stack
  // but $rbx is pushed on stack on x86_64 due to the later function call.
  // The function is therefore named as void *fp(void) with no internal storage.
  arch_int_t loc = 0;
  GET_SP_LOC(loc);
  gvar_init(gvar() - loc);
}

int main(int argc, char* argv[])
{
  arch_int_t loc = 0;
  GET_SP_LOC(loc);
  gvar_init(loc);

  helper_v_v();

  if(gvar() > 0){
    write_to_temp_file((int)gvar()/4 , argc, argv);
    return 64;
  }else{
    return 1;
  }
}
