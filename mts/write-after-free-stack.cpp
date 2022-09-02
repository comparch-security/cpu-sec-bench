#include "include/mss.hpp"

charBuffer *gbuffer;

void helper() {
  charBuffer buffer_stack[80];
  for(int i=0; i<80; i++)
    (buffer_stack+i)->updateBuffer('u', 'd', 'o');
  gbuffer = buffer_stack+40;
}

int main() {
  helper();
  update_by_pointer(gbuffer->data, 0, 8,  1, 'c');
  return check(*gbuffer, 8,  1, 'c', 1);
}
