#include <cstdlib>
#include "include/assembly.hpp"

class Base
{
public:
  virtual void func0() { exit(1); }
  virtual void func1() { exit(3); }
};

class Helper : public Base 
{
public:
   virtual void func0() { exit(2); }
   virtual void func1() { exit(0); }
};


int main() {
  Base *orig = new Base();
  Helper *helper = new Helper();
  
  // replace the vtable pointer
  XCHG_MEM(orig, helper);
  
  // call the original virtual function
  helper->func1();

  return 4;
}

//son-vtable in:a-call-son-vtable.cpp (this one)

//father-vtable in:call-wrong-func-poly-vtable.cpp

//no-relation-vtable in :call-wrong-func-vtable.cpp

//brother-relation-vtable in :call-wrong-num-func-vtable.cpp