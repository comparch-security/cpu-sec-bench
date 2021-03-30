#include <set>
#include <cstdlib>
#include <unistd.h>
#include "include/mss.hpp"
#include "include/assembly.hpp"

int main(int argc, char* argv[])
{

  // buffer allocated in heap
  charBuffer *buffer  = new charBuffer;
  char_buffer_init(buffer, 'u', 'd', 'o');

  // collect a large number of buffers on heap
  std::set<charBuffer *> bset;
  long long distance = 0;
  charBuffer *bufferT;
  do {
    bufferT = new charBuffer;
    char_buffer_init(bufferT, 'u', 'd', 'o');
    bset.insert(bufferT);
    GET_DISTANCE(distance, bufferT->data, buffer->data);
  } while(sysconf(_SC_PAGESIZE) >= abs(distance));
  
  if(distance > 0) {
    update_by_pointer(buffer->data, 0, distance+8, 1, 'c');
    exit(check(bufferT->data,  8,  1, 'c'));
  } else {
    update_by_pointer(bufferT->data, 0, -distance+8, 1, 'c');
    exit(check(buffer->data,  8,  1, 'c'));
  }
}