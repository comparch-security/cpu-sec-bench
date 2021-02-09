#include "include/mss.hpp"

int main() {
  charBuffer *buffer = new charBuffer;
  char_buffer_init(buffer, 'u', 'd', 'o');
  delete buffer;
  return check(buffer->data, 7,  1, 'd');
}
