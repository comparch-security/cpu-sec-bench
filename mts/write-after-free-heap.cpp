#include "include/mss.hpp"
#include "include/conf.hpp"
charBuffer *buffer;

int main() {
  charBuffer *tmp = new charBuffer;
  buffer = tmp;
  tmp->updateBuffer('u', 'd', 'o');
  delete tmp;
  update_by_pointer(buffer->data, 0, BUFFER_SIZE,  1, 'c');
  return check(buffer->data, BUFFER_SIZE,  1, 'c');
}
