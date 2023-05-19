#include "include/mss.hpp"

int main() {
  charBuffer *buffer = new charBuffer('u','d','o');
  delete buffer;
  return check(buffer->underflow, 7,  1, 'u') & check(buffer->data, 7,  1, 'd') & check(buffer->overflow, 7,  1, 'o');
}
