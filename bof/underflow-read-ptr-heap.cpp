#include "include/bof.hpp"

int main() {
  charBuffer *buffer = new charBuffer;
  char_buffer_init(buffer, 'u', 'd', 'o');
  return read_pointer(buffer->data, -8, 7, 1, 'u');
}
