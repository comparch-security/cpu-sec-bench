#include "include/mss.hpp"

int main() {
  charBuffer *buffer = new charBuffer('u','d','o');
  delete buffer;
  return check(*buffer, 7,  1, 'd',1);
}
