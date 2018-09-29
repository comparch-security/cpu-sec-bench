#include <cstdlib>
#include "include/assembly.hpp"
#include "include/cfi.hpp"

class HelperReplace : public HelperOrig
{
  int lvar;
public:
    virtual int virtual_func(int new_var) {
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
  pOrig->virtual_func();

  return 2;
 }


