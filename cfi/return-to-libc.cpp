#include <cstdlib>
#include "include/assembly.hpp"

#include <iostream>
using namespace std;


void FORCE_NOINLINE helper()
{
	asm("xor %rdi,%rdi");
	MOD_RET_DAT(exit);
}

int main()
{
	asm_stack_test();
	helper();
	return 3;
}