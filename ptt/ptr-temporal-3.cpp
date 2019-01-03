#include "include/ptt.hpp"

volatile char *t;

void foo() {
  volatile char c = 'c';
  t = &c;
}

int helper() {
  foo();
  if(*t == 'c') // except if pionter temporal protection exists
    return 0;
  else
    return 1;
}

int main()
{
  return helper();
}
