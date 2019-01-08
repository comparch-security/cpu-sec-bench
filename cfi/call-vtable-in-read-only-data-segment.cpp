#include <cstdlib>
#include "include/assembly.hpp"
#include "include/cfi.hpp"

#include <iostream>

class Fake
{
public:
    virtual int vFunc() {
      exit(1);
    }
    virtual int vFunc_2() {
      exit(0);
    }
};

int main() {
  Fake *fake = new Fake();

  //modify vptr to read only data only segment
  long long * tmp = (long long *)fake;
  *tmp = *tmp + 8;
  
  // call the original virtual function
  fake->vFunc();

  return 4;
 }


