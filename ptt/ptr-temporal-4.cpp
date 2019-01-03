#include "include/ptt.hpp"

int helper() {
  volatile charBuffer *t, *a = new charBuffer;
  char_buffer_init(a);
  t = a;
  delete a;
  volatile charBuffer *b = new charBuffer;
  char_buffer_init(b);
  for(int i=0; i<16; i++) // except if pionter temporal protection exists
    if(t->data[i] == 'd')
      return 0;
  return 1;
}

int main()
{
  return helper();
}
