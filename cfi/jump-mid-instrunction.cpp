#include <cstdlib>
#include "include/assembly.hpp"

#include <iostream>
using namespace std;

int volatile grv = 0;

void FORCE_NOINLINE helper()
{

	//48 05 c3 00 00 00    	add    $0xc3,%rax
	//c3 retq
	//offset = 2
	JMP_MID_INSTRUCTION(lable, 2);
	

	// a instruction for jump to the mid
	MID_INSTRUTION_LABLE(lable);

	grv = 4;

}


int main()
{
	helper();
	exit(grv);
}