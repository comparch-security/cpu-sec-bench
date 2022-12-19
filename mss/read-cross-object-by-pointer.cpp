#include "include/mss.hpp"
#include "include/assembly.hpp"

const charBuffer buffer_rodataU('u','d','o');
const charBuffer buffer_rodataM('u','d','o');
const charBuffer buffer_rodataD('u','d','o');

// buffer in data
charBuffer buffer_dataU('u','d','o'), buffer_dataM('u','d','o'), buffer_dataD('u','d','o');

int main(int argc, char* argv[])
{
  // buffer in local stack
  charBuffer buffer_stackU('u','d','o');
  charBuffer buffer_stackM('u','d','o');
  charBuffer buffer_stackD('u','d','o');

  // buffer allocated in heap
  charBuffer *buffer_heapU = new charBuffer('u','d','o');
  charBuffer *buffer_heapM = new charBuffer('u','d','o');
  charBuffer *buffer_heapD = new charBuffer('u','d','o');

  int store_type = argv[1][0] - '0';
  long long distance_up   = 0;
  long long distance_down = 0;

  int rv;

  switch(store_type) {
  case 0: // stack
    GET_DISTANCE(distance_up,   (long long)buffer_stackU.underflow, (long long)buffer_stackM.data);
    GET_DISTANCE(distance_down, (long long)buffer_stackD.overflow,  (long long)buffer_stackM.data);
    rv =
      read_by_pointer(buffer_stackM.data, distance_up,   7, 1, 'u') &
      read_by_pointer(buffer_stackM.data, distance_down, 7, 1, 'o') ;
    break;
  case 1: // heap
    GET_DISTANCE(distance_up,   (long long)buffer_heapU->underflow, (long long)buffer_heapM->data);
    GET_DISTANCE(distance_down, (long long)buffer_heapD->overflow,  (long long)buffer_heapM->data);
    rv =
      read_by_pointer(buffer_heapM->data, distance_up,   7, 1, 'u') &
      read_by_pointer(buffer_heapM->data, distance_down, 7, 1, 'o') ;
    break;
  case 2: // data
    GET_DISTANCE(distance_up,   (long long)buffer_dataU.underflow, (long long)buffer_dataM.data);
    GET_DISTANCE(distance_down, (long long)buffer_dataD.overflow,  (long long)buffer_dataM.data);
    rv =
      read_by_pointer(buffer_dataM.data, distance_up,   7, 1, 'u') &
      read_by_pointer(buffer_dataM.data, distance_down, 7, 1, 'o') ;
    break;
  case 3: // rodata
    GET_DISTANCE(distance_up,   (long long)buffer_rodataU.underflow, (long long)buffer_rodataM.data);
    GET_DISTANCE(distance_down, (long long)buffer_rodataD.overflow,  (long long)buffer_rodataM.data);
    rv =
      read_by_pointer(buffer_rodataM.data, distance_up,   7, 1, 'u') &
      read_by_pointer(buffer_rodataM.data, distance_down, 7, 1, 'o') ;
    break;
  default:
    rv = -1;
  }

  delete buffer_heapU;
  delete buffer_heapM;
  delete buffer_heapD;

  return rv;
}
