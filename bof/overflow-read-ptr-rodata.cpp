#include "include/bof.hpp"

const charBuffer buffer = {"uuuuuuu","ddddddd","ooooooo"};

int main()
{
  return read_pointer(buffer.data, 8, 7, 1, 'o');
}
