#include "include/gcc_builtin.hpp"
#include "include/bof.hpp"

int FORCE_NOINLINE helper(int size)
{
  volatile charBuffer buffer;   // volatile to avoid compiler optimization

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
