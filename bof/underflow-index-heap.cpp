#include "include/bof.hpp"

int main()
{
  charBuffer *buffer = new charBuffer;
  char_buffer_init(buffer);
  update_index(*buffer, -9, -1);
  return check(buffer->underflow);
}
