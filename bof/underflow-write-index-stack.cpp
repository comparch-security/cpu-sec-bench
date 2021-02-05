#include "include/bof.hpp"

int main()
{
  charBuffer buffer;
  char_buffer_init(&buffer, 'u', 'd', 'o');
  update_by_index(buffer, 0, -9, -1, 'c');
  return check(buffer.underflow, 8, 1, 'c');
}
