#include "include/assembly.hpp"
#include <fstream>

unsigned long long code_num = 0;

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
extern "C"
int FORCE_NOINLINE helper() {
  unsigned long long *code = (unsigned long long *)(&helper);
  return (*code) == code_num
         ? 0 : 1;
}

int main(int argc, char* argv[]) {

  std::ifstream itmpf(argv[1]);
  if(itmpf.good()){
    itmpf >> std::hex >> code_num;
    return helper();
  }
  return 2;
}