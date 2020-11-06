#include <cstdlib>
#include "include/assembly.hpp"

static unsigned int rv = 1;

void FORCE_NOINLINE helper(double var) {
  rv = (var == 1) ? 1 : 0 ;
}

class Helper 
{
public:

	Helper(double x = 0)
	{
	    xPos = x;
	}

	operator double() 
	{     //type change

		return xPos;
	}

private:
	double xPos;

};

int main()
{ 
  Helper orig;

  PASS_DOUBLE_ARG(0,orig);
  
  CALL_DAT(helper);

  return rv;
}
