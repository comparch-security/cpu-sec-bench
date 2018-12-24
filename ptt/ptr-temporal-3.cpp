#include "include/gcc_builtin.hpp"
#include "include/ptt.hpp"

volatile char *t;

void foo()
{
	volatile char c = 'c';
	t = &c;
}

int helper()
{
	foo();
	if(*t == 'c')  //if pionter temporal protection exist ,it will throw exception here
		return 0;
    else
		return 1;

}


int main()
{
	return helper();
}