#include <cstdlib>
#include "include/assembly.hpp"

static volatile int grv = 5;

void FORCE_NOINLINE helper() {
  grv += 5;
  DECL_LABEL(func_mid);

  grv -= 5;
  exit(grv);
}

int main() {
  CALL_LABEL(func_mid, 0);
  exit(grv);
}
