#include <cstdlib>
#include "include/assembly.hpp"

static volatile int grv = 0;

void FORCE_NOINLINE helper() {
  ENFORCE_NON_LEAF_FUNC;
  grv++;

  if(grv == 2) {
    MOD_RET_LABEL(helper2_ret);
    grv = 0;
  }
}

int main()
{
  // call a function but illegally return
  helper();
  // the elligal return site
  DECL_LABEL(helper2_ret);
  if(grv == 0)
    exit(grv);
  helper();
  helper();
  return grv;
}
