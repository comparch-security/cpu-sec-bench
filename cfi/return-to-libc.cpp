#include <cstdlib>
#include "include/assembly.hpp"

void FORCE_NOINLINE helper()
{
	XOR_RDI_RDI; 
	MOD_RET_DAT(exit);
}

int main()
{
	asm_stack_test();
	helper();
	return 3;
}