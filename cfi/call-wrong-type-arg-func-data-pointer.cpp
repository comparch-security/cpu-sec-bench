#include "include/assembly.hpp"

static unsigned int rv = 1;

void FORCE_NOINLINE helper(int* var) {
  rv = (*var == 0) ? 1 : 0 ;
}

void FORCE_NOINLINE helper2(){
	return;
}

int main()
{
	int var = 0;
	helper(&var);
	long long tmp = (long long)helper2;
    PASS_INT_ARG0(tmp);
    CALL_FUNC(helper);  
    return rv;
}
