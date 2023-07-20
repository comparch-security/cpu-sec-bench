#include <cstdlib>
#include <cstdint>
#include "include/assembly.hpp"

int main(int argc, char* argv[])
{
  int cet_enabled = argv[1][0] - '0';

  void *rand_label = &main;
  GET_LABEL_ADDRESS(rand_label,TARGET_LABEL);

  // In LLVM, goto is not allow to jump over declaration of local variables.
  void *got = NULL;

  if(cet_enabled == -1) { GOTO_SAVED_LABEL(rand_label,TARGET_LABEL);}   // impossible to happen

  get_got_func(&got, rand_label, cet_enabled);
  COMPILER_BARRIER;
TARGET_LABEL(argc)
  rand();
  COMPILER_BARRIER;
  return 0 != *(uintptr_t *)(got) ? 0 : 1;
}
