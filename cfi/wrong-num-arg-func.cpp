#include "include/assembly.hpp"

static volatile int grv = 1;

int helper(int a, int b) {
  grv = 0;
  return a+b;
}

int main() {
  CALL_VOID_FUNC_0(helper);
  return grv;
}

