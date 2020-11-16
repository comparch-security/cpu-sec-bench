#include <cstdlib>
#include "include/assembly.hpp"

static volatile int grv = 1;

void FORCE_NOINLINE helper() {
  grv += 5;
  DECL_LABEL(func_mid);

  grv -= 5;
  exit(grv);
}

int main() {
  grv = 5;
  CALL_LABEL(func_mid, 0);
  exit(grv);
}
