#include <cstdlib>
#include "include/assembly.hpp"

static unsigned int rv = 1;

class Base
{
public:
  virtual void func0() = 0;
  virtual void func1() = 0;
};

class Helper : public Base
{
public:
  virtual void func0() { exit(1); }
  virtual void func1() { exit(2); }
};


int main() {
  long long var = 0;
  Base *orig = new Helper();
  long long * p = &var; 
  p = (long long *)orig;  //read a vtable pointer

  rv = (*p == 0) ? 1 : 0 ;

  return rv;
}
