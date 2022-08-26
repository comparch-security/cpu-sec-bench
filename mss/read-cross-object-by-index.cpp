#include "include/mss.hpp"
#include "include/assembly.hpp"

const charBuffer buffer_rodataU('u','d','o');
const charBuffer buffer_rodataM('u','d','o');
const charBuffer buffer_rodataD('u','d','o');

// buffer in data
charBuffer buffer_dataU('u','d','o');
charBuffer buffer_dataM('u','d','o');
charBuffer buffer_dataD('u','d','o');

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

  switch(store_type) {
  case 0: // stack
    GET_DISTANCE(distance_up,   buffer_stackU.underflow, buffer_stackM.data);
    GET_DISTANCE(distance_down, buffer_stackD.overflow,  buffer_stackM.data);
    return
      read_by_index(buffer_stackM, distance_up,   7, 1, 'u') &
      read_by_index(buffer_stackM, distance_down, 7, 1, 'o') ;
  case 1: // heap
    GET_DISTANCE(distance_up,   buffer_heapU->underflow, buffer_heapM->data);
    GET_DISTANCE(distance_down, buffer_heapD->overflow,  buffer_heapM->data);
    return
      read_by_index(*buffer_heapM, distance_up,   7, 1, 'u') &
      read_by_index(*buffer_heapM, distance_down, 7, 1, 'o') ;
  case 2: // data
    GET_DISTANCE(distance_up,   buffer_dataU.underflow, buffer_dataM.data);
    GET_DISTANCE(distance_down, buffer_dataD.overflow,  buffer_dataM.data);
    return
      read_by_index(buffer_dataM, distance_up,   7, 1, 'u') &
      read_by_index(buffer_dataM, distance_down, 7, 1, 'o') ;
  case 3: // rodata
    GET_DISTANCE(distance_up,   buffer_rodataU.underflow, buffer_rodataM.data);
    GET_DISTANCE(distance_down, buffer_rodataD.overflow,  buffer_rodataM.data);
    return
      read_by_index(buffer_rodataM, distance_up,   7, 1, 'u') &
      read_by_index(buffer_rodataM, distance_down, 7, 1, 'o') ;
  default:
    return -1;
  }
}
