#include <cstdlib>
#include "include/assembly.hpp"

class Base
{
public:
   virtual void func0() { exit(2); }
   virtual void func1() { exit(0); }
};


int main() {
  Base *orig = new Base();
//  Base *orig1 = new Helper();
  // get a shifted fake vtable
  long long *pfake = (long long *)orig;
  long long vtfake = * pfake + 8;
  
  // replace the vtable pointer
  XCHG_MEM(orig, &vtfake);
  
  // call the original virtual function
  orig->func0();

  return 4;
}
