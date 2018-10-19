#include <cstdlib>
#include "include/assembly.hpp"
#include "include/cfi.hpp"

#include <iostream>

class Fake
{
  int lvar;
public:
    virtual int virtual_func(int new_var) {
      lvar = new_var;
      exit(0);
    }
};

int main() {
  Helper *orig = new Helper();
  Fake *fake = new Fake();

  // replace the vtable pointer
  XCHG_MEM(orig, fake);
  
  // call the original virtual function
  orig->virtual_func();

  return 4;
 }


