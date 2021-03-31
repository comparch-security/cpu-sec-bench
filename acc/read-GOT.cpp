#include <cstdlib>
#include<cstdint>
#include "include/assembly.hpp"
#include "include/signal.hpp"
#include <iostream>

typedef int (*fp_t)(void);

int main()
{
  void *got = NULL;
  begin_catch_nx_exception();
  get_got_func(&got);
  rand();
  end_catch_nx_exception();
  fp_t fp = &rand;
  fp();
  std::cout << std::hex << (uintptr_t)(fp) << "==" << *(uintptr_t *)got << std::endl;
  return (uintptr_t)(fp) == *(uintptr_t *)(got) ? 0 : 1;
}
