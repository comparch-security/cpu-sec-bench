#include <cstdlib>
#include "include/assembly.hpp"

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

class Fake : public Base
{
public:
  virtual void func0() { exit(3); }
  virtual void func1() { exit(0); }
};

int main()
{
  Base *orig = new Helper();
  Base *fake = new Fake();

  // get a shifted fake vtable
  long long *pfake = (long long *)fake;
  long long vtfake = *pfake + 8;

  // replace the vtable pointer
  XCHG_MEM(orig, &vtfake);
  
  // call the original virtual function
  orig->func0();

  // run fake to avoid warning
  fake->func0();

  return 4;
}
