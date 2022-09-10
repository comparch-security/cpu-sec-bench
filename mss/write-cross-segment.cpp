#include "include/mss.hpp"
#include "include/assembly.hpp"
#include <cstdio>
// buffer in data
charBuffer buffer_data('c', 'd', 'e');

int main(int argc, char* argv[])
{
  // buffer in local stack
  charBuffer buffer_stack('r','s','t');

  // buffer allocated in heap
  charBuffer *buffer_heap = new charBuffer('g','h','i');

  int src_type = argv[1][0] - '0';
  int des_type = argv[2][0] - '0';
  int acc_type = argv[3][0] - '0';
  long long distance   = 0;

  char *psrc, *pdes;

  switch(src_type) {
  case 0: psrc = buffer_stack.data; break;
  case 1: psrc = buffer_heap->data; break;
  case 2: psrc = buffer_data.data; break;
  }

  switch(des_type) {
  case 0: pdes = buffer_stack.data; break;
  case 1: pdes = buffer_heap->data; break;
  case 2: pdes = buffer_data.data;  break;
  }

  GET_DISTANCE(distance, pdes, psrc);

  if(acc_type == 0) {
    switch(src_type) {
    case 0: update_by_index(buffer_stack,  distance, 8, 1, 'c'); break;
    case 1: update_by_index(*buffer_heap,  distance, 8, 1, 'c'); break;
    case 2: update_by_index(buffer_data,   distance, 8, 1, 'c'); break;
    }
  } else {
    update_by_pointer(psrc, distance, 8, 1, 'c');
  }

  int rv = check(pdes, 8, 1, 'c');
  delete buffer_heap;
  return rv;
}
