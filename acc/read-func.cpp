#include "include/assembly.hpp"
#include <cstdlib>

unsigned int code_num    = 0;
unsigned int code_mask   = 0;
unsigned int inst_byte_offset = 0;
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

int FORCE_NOINLINE helper(int var) {
  unsigned char *code = (unsigned char *)(&helper);
  code += inst_byte_offset;
  return ((*(unsigned int*)code) & code_mask) == code_num
         ? 0 : 1;
}

int main(int argc, char* argv[]) {
  int var = argv[1][0] - '0';
  code_num         = (unsigned int)atoi(argv[2]);
  code_mask        = (unsigned int)atoi(argv[3]);
  inst_byte_offset = (unsigned int)atoi(argv[4]);

  return helper(var);
}