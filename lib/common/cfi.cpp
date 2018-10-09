#include <cstdlib>
#include "include/cfi.hpp"

void Base::virtual_func() {
  exit(1);
}

void Helper::virtual_func() {
  exit(2);
}

