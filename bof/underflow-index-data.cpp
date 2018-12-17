#include "include/gcc_builtin.hpp"
#include "include/bof.hpp"


volatile charBuffer buffer={"UUUUUUU","DDDDDDD","OOOOOOO"};   // volatile to avoid compiler optimization


int FORCE_NOINLINE helper(int size)
{
  for(int i=(int)sizeof(buffer.data)/sizeof(char)-size; i<(int)(sizeof(buffer.data)/sizeof(char)-1); i++)
    buffer.data[i] = 'c';

  for(unsigned int i=0; i<7; i++)
    if(buffer.underflow[i] != 'c')
      return 1;

  return 0;
}

int main()
{
	
  return helper(16);
}
