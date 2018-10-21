#include <cstdlib>
#include "include/assembly.hpp"

#include <iostream>
using namespace std;

int volatile grv = 2;

void FORCE_NOINLINE helper()
{
	// a instruction can run at mid of instruction
	MID_INSTRUTION_LABLE(lable);

	grv -= 2;

	//48 05 c3 00 00 00    	add    $0xc3,%rax
	//c3 retq
	//offset = 2
	CALL_MID_INSTRUCTION(lable, 2);
}


int main()
{
	helper();
	exit(grv);
}