#include "include/ptt.hpp"

int main() {
  char *buffer;
  return get_reallocated_block(&buffer, false) ? 0 : 1;
}
