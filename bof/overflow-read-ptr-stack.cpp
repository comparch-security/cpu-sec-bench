#include "include/bof.hpp"

int main()
{
  charBuffer buffer;
  char_buffer_init(&buffer, 'u', 'd', 'o');
  return read_by_pointer(buffer.data, 8, 7, 1, 'o');
}
