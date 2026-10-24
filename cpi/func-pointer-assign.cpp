#include "include/assembly.hpp"
#include "include/result.hpp"
#include <cstdint>

static volatile int grv = 1;

void FORCE_NOINLINE helper() {
  grv--;
}

typedef void(*func_t)();

void FORCE_NOINLINE call_helper(intptr_t fp) {
  func_t func = reinterpret_cast<func_t>(fp);
  func();
}

int main() {
  intptr_t fp = reinterpret_cast<intptr_t>(&helper);
  call_helper(fp);
  return grv == 0 ? RT_CODE_SUCCESS : 1; // 0: helper was reached through the assigned pointer
}
