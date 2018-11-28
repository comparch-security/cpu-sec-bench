#include "include/assembly.hpp"
#include "include/gcc_builtin.hpp"
#include "include/signal.hpp"

int volatile grv = 1;

int main()
{
  unsigned char m[] = FUNC_MACHINE_CODE;
  signal(SIGSEGV, sigsegv_handler); // catch SIGSEGV
  PUSH_LABEL(lable);
  JMP_FUNC(m);
  DECL_LABEL(lable);
  grv = 0;
  return grv;
}
