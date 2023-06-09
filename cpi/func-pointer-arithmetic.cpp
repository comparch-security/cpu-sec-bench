#include "include/assembly.hpp"
#include <cstdint>

static volatile int grv = 1;

void FORCE_NOINLINE helper() {
  grv--;
}

typedef void(*func_t)();

void FORCE_NOINLINE call_helper(intptr_t fp, intptr_t diff) {
  func_t func = reinterpret_cast<func_t>(fp);
  func = (func_t)((long long)func +diff);
  func();
}

int main() {
  intptr_t fp = reinterpret_cast<intptr_t>(&call_helper);
  intptr_t diff = reinterpret_cast<intptr_t>(&helper) - fp;
  call_helper(fp, diff);
  return grv;
}
