#include <cstdlib>
#include "include/assembly.hpp"

int grv = 0;

int FORCE_NOINLINE helper()
{
	//modify return address to mid of instruction
	MOD_RET_LABEL(mid_instruction + 2);
	return 1;
}

int FORCE_NOINLINE helper2()
{
	//modify rsp
	POP_STACK;
	asm_stack_test();
	helper();
	//add $0xc3, %rax
	//ret （address + 2） ==> c3 00 00(ret)
	MID_INSTRUTION;
	grv++;
	return 1;
}

int main()
{
	helper2();
	return grv;
}

