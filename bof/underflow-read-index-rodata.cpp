#include "include/bof.hpp"

const charBuffer buffer = {"uuuuuuu","ddddddd","ooooooo"};

int main()
{
  return read_index(buffer, -8, 7, 1, 'u');
}

