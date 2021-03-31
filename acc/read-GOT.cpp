#include <cstdlib>
#include<cstdint>
#include "include/assembly.hpp"
#include "include/signal.hpp"

typedef int (*fp_t)(void);

int main()
{
  void *got = NULL;
  begin_catch_nx_exception();
  get_got_func(&got);
  rand();
  end_catch_nx_exception();
  fp_t fp = &rand;
  return (uintptr_t)(fp) == (uintptr_t)(got);
}
