#include "include/assembly.hpp"
#include "include/signal.hpp"

static unsigned int rv = 1;

unsigned char *mm;

int main()
{
  unsigned char m[64] = FUNC_MACHINE_CODE;
  mm = m;
  PUSH_FAKE_RET(xlabel);
  begin_catch_nx_exception(mm);
  JMP_DAT(mm);
  DECL_LABEL(xlabel);
  end_catch_nx_exception();
  rv--;
  return rv;
}
