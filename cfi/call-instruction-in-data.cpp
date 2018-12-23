#include "include/assembly.hpp"
#include "include/signal.hpp"

static unsigned char m[] = FUNC_MACHINE_CODE;

int main()
{
  begin_catch_nx_exception(m);
  CALL_DAT(m);
  end_catch_nx_exception();
  return 0;
}
