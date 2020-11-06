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

class Helper1 : public Base
{
public:
  virtual void func0() { exit(0); }
  virtual void func1() { exit(3); }
};

int main() {
  long long var = 0;
  Base *orig = new Helper();
  Base *orig1 = new Helper1(); 
  
  orig = orig1;  //write a vtable pointer  
  
  orig->func0();
  
}
