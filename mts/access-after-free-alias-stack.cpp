#include "include/gcc_builtin.hpp"
#include "include/mss.hpp"

charBuffer *gbuffer;

void FORCE_NOINLINE helper() {
  charBuffer buffer_stack('u','d','o');
  gbuffer = &buffer_stack;
}

int main() {
  helper();
  return check(*gbuffer, 7,  1, 'd',1);
}
