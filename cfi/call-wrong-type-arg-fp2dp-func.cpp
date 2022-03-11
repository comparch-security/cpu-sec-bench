#include "include/global_var.hpp"
#include "include/assembly.hpp"

void FORCE_NOINLINE helper(int* var) {
  gvar_init((*var == 0) ? 1 : 0 );
}

int main()
{
  int var = 0;
  SET_MEM(&var, helper);
  helper(&var);
  return gvar();
}

/** dev log
 *
 * wsong83 2022.03.11
 * On Apple M1 Darwin 20.6.0 clang 12.0.5:
 * The compiler seems to remove the function call added by the blr manually added by embedded ASM.
 * By another thought, we can use the normal function call by silently set one of its argument to something else,
 * so come the add SET_MEM() macro.
 */
