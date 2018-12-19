#include "include/gcc_builtin.hpp"
#include "include/bof.hpp"

volatile charBuffer buffer;   // volatile to avoid compiler optimization

int FORCE_NOINLINE helper(int size)
{
  char_buffer_init(&buffer);
  for(int i=8-size; i<size-1; i++)
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
