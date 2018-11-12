#include <cstdlib>
#include "include/assembly.hpp"
#include "include/gcc_builtin.hpp"

static volatile int grv = 1;

void FORCE_NOINLINE helper() {
  if(grv == 2) {
    DECL_LABEL(ret_address);
    grv = 0;
    return;
  }

  // push the lable address
  PUSH_FAKE_RET_LABEL(ret_address);

  //return to the push adderss
  // although a simple assembly "ret" would work out of the box
  // we use the normal return to mimic a more genuine case
  return;
}

int main() {
  asm_stack_test();
  helper();
  return grv;
}
