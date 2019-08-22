#include "include/cfi.hpp"

int main()
{
  long long var = 0;
  Base *orig = new Helper();
  long long *vtp = (long long *)(*(long long *)(orig));
  return (*vtp == 0) ? 1 : 0;
}
