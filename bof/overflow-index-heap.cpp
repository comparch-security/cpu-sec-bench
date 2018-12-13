#include "include/gcc_builtin.hpp"
#include "include/bof.hpp"


int FORCE_NOINLINE helper(unsigned int size)
{
  volatile charBuffer * buffer=new charBuffer();   // volatile to avoid compiler optimization

  for(unsigned int i=0; i<size-1; i++)
    buffer->data[i] = 'c';

  for(unsigned int i=0; i<7; i++)
    if(buffer->overflow[i] != 'c')
    {
      delete buffer;
      return 1;
    }
  delete buffer;
  return 0;
}

int main()
{
  return helper(16);
}
