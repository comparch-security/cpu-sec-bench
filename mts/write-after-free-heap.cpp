#include "include/mss.hpp"

charBuffer *buffer;

int main() {
  charBuffer *tmp = new charBuffer;
  buffer = tmp;
  tmp->updateBuffer('u', 'd', 'o');
  delete tmp;
  update_by_pointer(buffer->data, 0, 8,  1, 'c');
  return check(buffer->data, 8,  1, 'c');
}
