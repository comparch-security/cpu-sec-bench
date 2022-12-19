#include "include/mss.hpp"
#include "include/assembly.hpp"
#include <cstdio>

const charBuffer buffer_rodata('q','r','s');
// buffer in data
charBuffer buffer_data('c','d','e');

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

  const char *psrc, *pdes;
  char tc;

  switch(src_type) {
  case 0: psrc = buffer_stack.data; break;
  case 1: psrc = buffer_heap->data; break;
  case 2: psrc = buffer_data.data; break;
  case 3: psrc = buffer_rodata.data; break;
  }

  switch(des_type) {
  case 0: pdes = buffer_stack.data;  tc = 's'; break;
  case 1: pdes = buffer_heap->data;  tc = 'h'; break;
  case 2: pdes = buffer_data.data;   tc = 'd'; break;
  case 3: pdes = buffer_rodata.data; tc = 'r'; break;
  }

  GET_DISTANCE(distance, (long long)pdes, (long long)psrc);

  int rv;

  if(acc_type == 0) {
    switch(src_type) {
    case 0: rv = read_by_index(buffer_stack,  distance, 7, 1, tc); break;
    case 1: rv = read_by_index(*buffer_heap,  distance, 7, 1, tc); break;
    case 2: rv = read_by_index(buffer_data,   distance, 7, 1, tc); break;
    case 3: rv = read_by_index(buffer_rodata, distance, 7, 1, tc); break;
    }
  } else {
    rv = read_by_pointer(psrc, distance, 7, 1, tc);
  }

  delete buffer_heap;
  return rv;
}
