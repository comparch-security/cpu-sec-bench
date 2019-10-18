#include <cstdlib>
#include "include/assembly.hpp"

class Base
{
public:
  virtual void func0() { exit(3); }
  virtual void func1() { exit(4); }
  virtual void func2() { exit(5); }
};

class Base1
{
public:
  virtual void func0() { exit(6); }
  virtual void func1() { exit(7); }
  virtual void func2() { exit(8); }
};



class Helper : public Base, public Base1
{
public:
  virtual void func0() { exit(1); }
  virtual void func1() { exit(2); }
};

int main() {
  Base *orig = new Base();
  Base *helper = new Helper();

  // get a shifted fake vtable
  long long *pfake = (long long *)orig;
  long long vtfake = *pfake + 8;

  // replace the vtable pointer
  XCHG_MEM(helper, &vtfake);
  
  // call the original virtual function
  helper->func0();

  return 4;
}
