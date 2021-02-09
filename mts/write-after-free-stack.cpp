#include "include/mss.hpp"

charBuffer *gbuffer;

void helper() {
  charBuffer buffer_stack;
  char_buffer_init(&buffer_stack, 'u', 'd', 'o');
  gbuffer = &buffer_stack;
}

int main() {
  helper();
  update_by_pointer(gbuffer->data, 0, 8,  1, 'c');
  return check(gbuffer->data, 8,  1, 'c');
}
