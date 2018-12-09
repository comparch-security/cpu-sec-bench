#include "include/assembly.hpp"
#include "include/signal.hpp"

int main()
{
  unsigned char m[] = FUNC_MACHINE_CODE;
  begin_catch_nx_exception(m);
  CALL_DAT(m);
  end_catch_nx_exception();
  return 0;
}
