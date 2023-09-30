#include "include/mss.hpp"
#include "include/conf.hpp"

charBuffer *gbuffer;

void helper() {
  charBuffer buffer_stack[80];
  for(int i=0; i<80; i++)
    (buffer_stack+i)->updateBuffer('u', 'd', 'o');
  gbuffer = buffer_stack+40;
}

int main() {
  helper();
  update_by_pointer(gbuffer->data, 0, BUFFER_SIZE,  1, 'c');
  return check(gbuffer->data, BUFFER_SIZE,  1, 'c');
}
