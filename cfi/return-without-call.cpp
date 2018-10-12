#include <cstdlib>
#include "include/assembly.hpp"

static volatile int grv = 0;

int main() {

	// push the lable address
	PUSH_LABLE(ret_address);

	//return to the push adderss
	RET;


  grv = 1; // failed if runs here

  DECL_LABEL(ret_address);

  exit(grv);
}