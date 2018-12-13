#include "include/assembly.hpp"
#include "include/signal.hpp"

int main()
{
  unsigned char *m = new unsigned char [16];
  assign_fake_machine_code(m);
  begin_catch_nx_exception(m);
  CALL_DAT(m);
  end_catch_nx_exception();
  return 0;
}
