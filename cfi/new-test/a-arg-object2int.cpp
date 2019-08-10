#include <cstdlib>
#include "include/assembly.hpp"

static unsigned int rv = 1;

void FORCE_NOINLINE helper(int var) {
  rv = (var == 1) ? 1 : 0 ;
}

class Helper 
{
public:

	Helper(int x = 0)
	{
	    xPos = x;
	}

	operator int() 
	{     //type change

		return xPos;
	}

private:
	int  xPos;

};

int main()
{ 
  Helper orig;

  PASS_INT_ARG(0, orig);
  
  CALL_DAT(helper);

  return rv;
}
