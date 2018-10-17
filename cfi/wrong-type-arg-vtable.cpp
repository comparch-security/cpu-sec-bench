#include <cstdlib>
#include "include/assembly.hpp"
#include "include/cfi.hpp"

#include <iostream>

class Fake : public Base
{
  double lvar;
public:
    virtual int virtual_func(double new_var) {
      lvar = new_var;
      exit(0);
    }
};

int main() {
  Helper2 *orig = new Helper2();
  Fake *fake = new Fake();

  // replace the vtable pointer
  XCHG_MEM(orig, fake);
  
  // call the original virtual function
  int val = 1;
  orig->virtual_func(val);

  return 4;
 }


