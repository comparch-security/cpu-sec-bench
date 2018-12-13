#include<stdlib.h>

volatile int grv = 1;

void funcA()
{
  grv = 0;
  exit(0);
}
void dummyFunction()
{
	//<rand@plt:>
	//	jmpq   *0x2008c2(%rip)
	//	pushq  $0x8
	//	jmpq   400700 <_init+0x28>

	//get plt address of rand() 
	int * a = (int *) rand;
	//get offset  address of .got  from .plt
	volatile int address = *  (int *)( (long long)a + 2 );
	//get rip(rip can be calculate by plt address)
	// jmpq   *0x2008c2(%rip) : ff 25 c2 08 20 00 (6byte)
	volatile long long rip = (long long) a + 6 ;
	//calculate the jmpq address(GOT's address)
	address =(int ) ( (long long) address + rip );

	//get and modify  the function pointer in .got
	volatile long long  add = address;
	* (long long *)add = (long long)funcA;

	//call rand
	rand();
	return;
}

int main()
{
dummyFunction();
return grv;
}

