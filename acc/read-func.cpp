#include "include/assembly.hpp"

/*
 * If a local variable is modified by an embedded assembly,
 * it might be removed by Clang 12 on Mac M1.
 *
 * When the targeted add (var += num) is enforced using volatile,
 * x86-64 might read the variable from the stack after the CHECK_POS label.
 *
 * On RV64GC, HiFive Unmatched, the GCC add a li assignment before the add.
 *
 * Consequently, we use a function to force the register assignement by ABI,
 * also set the sum using arguemnt to avoid imm assignemnt in RV64GC.
 * The add operation is plain C++ statement to avoid instruction removal
 * in Clang 12.
 *
 * When Intel CET is enabled,
 * the CHECK_POS returned by GCC might point to the endbr64 before var += cet
 * or correctly to var += cet.
 * Relax the check by reading both locations.
 */

int FORCE_NOINLINE helper(int var, int cet, int sum) {
  unsigned int *code = (unsigned int *)(&&CHECK_POS);
  unsigned int *code_cet = code + cet;
  mbarrier;
 CHECK_POS:
  var += cet;
  mbarrier;
  return (var == sum &&
          (
           ((*code    ) & READ_FUNC_MASK) == READ_FUNC_CODE ||
           ((*code_cet) & READ_FUNC_MASK) == READ_FUNC_CODE
          )
         )
    ? 0 : 1;
}

int main(int argc, char* argv[]) {
  int var = argv[1][0] - '0';
  int cet = argv[2][0] - '0';
  return helper(var, cet, var+cet);
}
