#include "include/bof.hpp"

charBuffer buffer;

int main()
{
  char_buffer_init(&buffer, 'u', 'd', 'o');
  return read_pointer(buffer.data, -8, 7, 1, 'u');
}
