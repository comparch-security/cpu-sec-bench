#include <cstdlib>
#include "include/global_var.hpp"
#include "include/assembly.hpp"

void FORCE_NOINLINE helper(void *ra_label) {
  ENFORCE_NON_LEAF_FUNC;
  volatile void *ra_stack = NULL;
  READ_STACK_DAT_IMM(ra_stack, 0);
  if(ra_stack == ra_label) { gvar_init(0); return; }
  READ_STACK_DAT_IMM(ra_stack, 4);
  if(ra_stack == ra_label) { gvar_init(1); return; }
  READ_STACK_DAT_IMM(ra_stack, 8);
  if(ra_stack == ra_label) { gvar_init(2); return; }
  READ_STACK_DAT_IMM(ra_stack, 12);
  if(ra_stack == ra_label) { gvar_init(3); return; }
  READ_STACK_DAT_IMM(ra_stack, 16);
  if(ra_stack == ra_label) { gvar_init(4); return; }
  READ_STACK_DAT_IMM(ra_stack, 20);
  if(ra_stack == ra_label) { gvar_init(5); return; }
  READ_STACK_DAT_IMM(ra_stack, 24);
  if(ra_stack == ra_label) { gvar_init(6); return; }
  READ_STACK_DAT_IMM(ra_stack, 28);
  if(ra_stack == ra_label) { gvar_init(7); return; }
  READ_STACK_DAT_IMM(ra_stack, 32);
  if(ra_stack == ra_label) { gvar_init(8); return; }
  READ_STACK_DAT_IMM(ra_stack, 36);
  if(ra_stack == ra_label) { gvar_init(9); return; }
  READ_STACK_DAT_IMM(ra_stack, 40);
  if(ra_stack == ra_label) { gvar_init(10); return; }
  READ_STACK_DAT_IMM(ra_stack, 44);
  if(ra_stack == ra_label) { gvar_init(11); return; }
  READ_STACK_DAT_IMM(ra_stack, 48);
  if(ra_stack == ra_label) { gvar_init(12); return; }
  READ_STACK_DAT_IMM(ra_stack, 52);
  if(ra_stack == ra_label) { gvar_init(13); return; }
  READ_STACK_DAT_IMM(ra_stack, 56);
  if(ra_stack == ra_label) { gvar_init(14); return; }
  READ_STACK_DAT_IMM(ra_stack, 60);
  if(ra_stack == ra_label) { gvar_init(15); return; }
  gvar_init(-1);
  return;
}

int main(int argc, char* argv[])
{
  void *ra_label = &&RA_POS;
  if(0 == argv[1][0] - '0') goto *ra_label;
  helper(ra_label);
  COMPILER_BARRIER;
 RA_POS:
  if(gvar() == -1) return 1;
  return gvar() + 32;
}

