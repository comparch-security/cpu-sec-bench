#include <cstdlib>
#include "include/assembly.hpp"

static volatile int grv = 2;

void FORCE_NOINLINE helper() {
  grv++;
  DECL_LABEL(func_mid);
  COMPILER_BARRIER;
  grv--;
  exit(grv);
}

int main() {
  grv--;
  CALL_LABEL(func_mid, 0);
  exit(grv);
}
