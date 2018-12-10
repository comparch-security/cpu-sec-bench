#include <cstdlib>
#include "include/assembly.hpp"
#include "include/gcc_builtin.hpp"

static volatile int grv = 1;

void FORCE_NOINLINE helper1()
{
	grv = 100;
}

void FORCE_NOINLINE helper2()
{
	grv = 0;
}

int main()
{
	typedef void (*Fun)(void);
	Fun pFun;
	Fun tmp = helper2;
	int b = 1024;
	if(b - grv)
	{
		pFun = helper1;
	}
	else
	{
		pFun = helper2;
	}
	XCHG_MEM(&tmp, &pFun);
	pFun();
	pFun();
	return grv;
}

