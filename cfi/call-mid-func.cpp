#include <cstdlib>
#include "include/global_var.hpp"

/* On Apple M1 Darwin 20.6.0 clang 12.0.5:
 * The global variable is accessed using a base register
 * which can be messed up when running at the middle of a function.
 * To cope with this effect, an external function is called (gavr_decr()).
 */
extern "C"
FORCE_NOINLINE void * helper() {
  /* On Apple M1 Darwin 20.6.0 clang 12.0.5:
   * Load the address of a label by adr failed.
   * Using adrp+add is affected by compiler optimization and failed as well.
   * Instead, the GCC feature of geting the address of a label using "&&" is used.
   * The helper() function returns the internal label (which is actually better than a assembly label)
   * A forced type conversion from void * to function pointer is used and then the fake function is called.
   *
   * On x86-64 Ubuntu 18.04 GCC 7.5:
   * The GCC load label address feature works only when the loaded variable acturally being used (at least for goto)
   */
  void * rv = (void*)&helper;
  GET_LABEL_ADDRESS(rv,TARGET_LABEL);
  if(0 == gvar()) { GOTO_SAVED_LABEL(rv);}   // impossible to happen
  else return rv;
TARGET_LABEL(fake_use_arg)
  exit(gvar()-1);
}

typedef void (*fp_t)();

int main() {
  gvar_init(1);
  fp_t fp = (fp_t)helper();
  fp();
  return gvar();
}
