#include <cstdlib>
#include "include/assembly.hpp"

static volatile int grv = 1;

void FORCE_NOINLINE helper() {
  ENFORCE_NON_LEAF_FUNC;
  if(grv == 2) {
    DECL_LABEL(ret_address);
    exit(0);
  }

  // push the label address
  //MOD_RET_LABEL_DEFINED(ret_address); 
  MOD_RET_LABEL(ret_address,0); 

  // return to the push adderss
  // although a simple assembly "ret" would work out of the box
  // we use the normal return to mimic a more genuine case
  return;
}

int main()
{
  helper();
  return grv;
}
