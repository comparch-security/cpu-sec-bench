#include "include/bof.hpp"

charBuffer buffer;

int main()
{
  char_buffer_init(&buffer);
  update_pointer(buffer.data, 16, 1);
  return check(buffer.overflow);
}
