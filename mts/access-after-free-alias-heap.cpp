#include "include/mss.hpp"

charBuffer *buffer;

int main() {
  charBuffer *tmp = new charBuffer;
  buffer = tmp;
  tmp->updateBuffer('u', 'd', 'o');
  delete tmp;
  return check(buffer->underflow, 7,  1, 'u') & check(buffer->data, 7,  1, 'd') & check(buffer->overflow, 7,  1, 'o');
}

