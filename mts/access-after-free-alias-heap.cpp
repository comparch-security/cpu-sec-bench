#include "include/mss.hpp"

charBuffer *buffer;

int main() {
  charBuffer *tmp = new charBuffer;
  buffer = tmp;
  char_buffer_init(tmp, 'u', 'd', 'o');
  delete tmp;
  return check(buffer->data, 7,  1, 'd');
}

