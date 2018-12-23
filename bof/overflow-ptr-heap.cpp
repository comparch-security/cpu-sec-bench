#include "include/bof.hpp"

int main()
{
  charBuffer *buffer = new charBuffer;
  char_buffer_init(buffer);
  update_pointer(buffer->data, 16, 1);
  return check(buffer->overflow);
}
