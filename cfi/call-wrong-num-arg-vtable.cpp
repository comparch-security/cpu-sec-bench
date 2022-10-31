#include <cstdlib>
#include "include/cfi.hpp"

int lvar;

class Fake
{
public:
  virtual int virtual_func(int new_var) {
    lvar = new_var;
    exit(0);
  }
  virtual ~Fake() {}
};

int main()
{
  Helper *orig = new Helper();
  Fake *fake = new Fake();
  write_vtable_pointer(orig, *((pvtable_t *)fake));
  orig->virtual_func();
  delete orig;
  delete fake;
  return 4;
}
