#include "include/bof.hpp"

int main()
{
  charBuffer buffer;
  char_buffer_init(&buffer);
  update_pointer(buffer.data, -9, -1);
  return check(buffer.underflow);
}