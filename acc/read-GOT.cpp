#include <cstdlib>
#include<cstdint>
#include "include/assembly.hpp"
#include "include/signal.hpp"

int main()
{
  void *got = NULL;
  begin_catch_nx_exception();
  get_got_func(&got);
  rand();
  end_catch_nx_exception();
  mbarrier;
  return 0 != *(uintptr_t *)(got) ? 0 : 1;
}
