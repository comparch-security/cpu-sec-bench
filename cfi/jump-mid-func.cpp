#include <cstdlib>
#include "include/global_var.hpp"
#include "include/assembly.hpp"

extern "C"
FORCE_NOINLINE void * helper(int a) {
  /* On Xeon 5220 Ubuntu 18.04 g++ 7.5.0
   * Directly return the address of a label using return &&helper_mid does not work
   * as g++ seems to think the address is useless outside the function
   * and directly return NULL.
   * So we set a variable for it and force a use of the variable to
   * make sure the label address is returned as expected.
   */
  void * lp = (void*)&helper;
  GET_LABEL_ADDRESS(lp,TARGET_LABEL);
  WRITE_TRACE("The Mid Address of Func: 0x", lp);
  switch(a) {
  case 3: return NULL; // make sure the return value is not constant (rv speculation)
  case 2: { GOTO_SAVED_LABEL(lp);}   // impossible to happen
  case 1: return lp;
  }
TARGET_LABEL(a) // illegal jump target
    exit(gvar() - 2);
}

int main(int argc, char* argv[])
{
  INIT_TRACE_FILE;
  gvar_init(2);
  void *target = helper(argv[1][0] - '0');

  /* On Apple M1 Darwin 20.6.0 clang 12.0.5:
   * The compiler seems to remove the indirect jump added manually added by embedded ASM.
   * Now we ask a helper to return the address of a mid-of-func label,
   * and then jump to this return address (JMP_LABEL() -> JMP_DAT()).
   */
  JMP_DAT(target);
  WRITE_TRACE("Successful Jumped", "");
  return gvar();
}
