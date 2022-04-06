#include "include/global_var.hpp"
#include "include/assembly.hpp"

void FORCE_NOINLINE helper(int* var) {
  gvar_init((*var == 0) ? 1 : 0 );
}

/* On Xeon 5220 Ubuntu 18.04 g++ 7.5.0
 * Since the local var is initialized to 0 and no compiler observable modification,
 * no stack space is allocated for it and the SET_MEM macro actully rewrite the canary added by the compiler,
 * which leads to stack smatching detection.
 * Declare the variable as a static global one instead.
 */
static int var;

int main()
{
  /* On Apple M1 Darwin 20.6.0 clang 12.0.5:
   * The compiler seems to remove the function call added by the blr manually added by embedded ASM.
   * By another thought, we can use the normal function call by silently set one of its argument to something else,
   * so come the add SET_MEM() macro.
   */
  SET_MEM(&var, helper);
  helper(&var);
  return gvar();
}
