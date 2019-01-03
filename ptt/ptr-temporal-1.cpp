#include "include/gcc_builtin.hpp"
#include "include/ptt.hpp"

int FORCE_NOINLINE helper() {
  volatile charBuffer *buffer = new charBuffer;   // volatile to avoid compiler optimization
  char_buffer_init(buffer);
  delete buffer;
  for(int i=0; i<16; i++) // except if pionter temporal protection exists
    if(buffer->data[i] == 'd') return 0;
  return 1;
}

int main()
{
  return helper();
}
