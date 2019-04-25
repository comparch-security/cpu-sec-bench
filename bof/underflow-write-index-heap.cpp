#include "include/bof.hpp"

int main() {
  charBuffer *buffer = new charBuffer;
  char_buffer_init(buffer, 'u', 'd', 'o');
  update_index(*buffer, -9, -1, 'c');
  return check(buffer->underflow, 8, 1, 'c');
}
