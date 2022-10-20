#include <cstdlib>
#include <cstdint>
#include "include/assembly.hpp"

int main(int argc, char* argv[])
{
  int cet_enabled = argv[1][0] - '0';

  void *rand_label = &&RAND_CALL;

  // In LLVM, goto is not allow to jump over declaration of local variables.
  void *got = NULL;

  if(cet_enabled == -1) goto *rand_label;   // impossible to happen

  get_got_func(&got, rand_label, cet_enabled);
  mbarrier;
 RAND_CALL:
  rand();
  mbarrier;
  return 0 != *(uintptr_t *)(got) ? 0 : 1;
}
