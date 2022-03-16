#include <cstdlib>
#include "include/global_var.hpp"
#include "include/assembly.hpp"

void * FORCE_NOINLINE helper(int a) {
  switch(a) {
  case 3: return NULL; // make sure the return value is not constant (rv speculation)
  case 2: goto helper_mid; // fake use of the label
  case 1: return &&helper_mid;
  }
helper_mid: // illegal jump target
    exit(gvar() - 2);
}

int main(int argc, char* argv[])
{
  gvar_init(2);
  void *target = helper(argv[1][0] - '0');
  JMP_DAT(target);
  return gvar();
}

/** dev log
 *
 * wsong83 2022.03.11
 * On Apple M1 Darwin 20.6.0 clang 12.0.5:
 * The compiler seems to remove the indirect jump added manually added by embedded ASM.
 */
