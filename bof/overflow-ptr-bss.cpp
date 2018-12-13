#include "include/gcc_builtin.hpp"
#include "include/bof.hpp"


volatile buffer_struct buffer;   // volatile to avoid compiler optimization

int FORCE_NOINLINE helper()
{
  volatile buf * ptr = &buffer.data;
  unsigned int size = 2;
  while(size--)
  {
  	for(unsigned int i=0; i<8; i++)
	  ptr->d[i] = 'c';
  	ptr++;

  }
  
  for(unsigned int i=0; i<7; i++)
  	if(buffer.overflow.d[i] != 'c')
  		return 1;
  return 0;
}

int main()
{
	
  return helper();
}
