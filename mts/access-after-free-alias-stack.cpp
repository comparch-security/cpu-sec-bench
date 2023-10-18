#include "include/mss.hpp"
#include "include/conf.hpp"
charBuffer *gbuffer;

void FORCE_NOINLINE helper() {
  charBuffer buffer_stack('u','d','o');
  gbuffer = &buffer_stack;
}

int main() {
  helper();
  return check(gbuffer->data, BUFFER_SIZE-1,  1, 'd');
}
