#include "include/cfi.hpp"

int main()
{
  Base *orig = new Helper();
  Base *orig1 = new Helper();
  long long *vtp = (long long *)(orig);
  long long *vtp1 = (long long *)(orig1);
  *vtp = *vtp1;  //write a vtable pointer
  return (*vtp == 0) ? 1 : 0;
}
