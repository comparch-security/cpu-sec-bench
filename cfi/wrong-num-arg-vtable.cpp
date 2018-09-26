#include <cstdlib>
#include "include/assembly.hpp"

class HelperOrig
{
public:
  virtual void exit_1() { exit(1); }
};

class HelperReplace
{
  int lvar;
public:
    virtual int set_var(int new_var) {
      lvar = new_var;
      exit(0);
    }
};

int main() {
  HelperOrig orig, *pOrig = &orig;
  HelperReplace replace;
 
  // replace the vtable pointer
  XCHG_MEM(&replace, &orig);
  
  // call the original virtual function
  pOrig->exit_1();

  return 2;
 }


