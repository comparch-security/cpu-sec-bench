#include "include/mss.hpp"
#include "include/assembly.hpp"

const charBuffer buffer_rodataU = {"uuuuuuu","ddddddd","ooooooo"};
const charBuffer buffer_rodataM = {"uuuuuuu","ddddddd","ooooooo"};
const charBuffer buffer_rodataD = {"uuuuuuu","ddddddd","ooooooo"};

charBuffer buffer_dataU, buffer_dataM, buffer_dataD;

int main(int argc, char* argv[])
{
  // buffer in data
  char_buffer_init(&buffer_dataU, 'u', 'd', 'o');
  char_buffer_init(&buffer_dataM, 'u', 'd', 'o');
  char_buffer_init(&buffer_dataD, 'u', 'd', 'o');

  // buffer in local stack
  charBuffer buffer_stackU;
  charBuffer buffer_stackM;
  charBuffer buffer_stackD;
  char_buffer_init(&buffer_stackU, 'u', 'd', 'o');
  char_buffer_init(&buffer_stackM, 'u', 'd', 'o');
  char_buffer_init(&buffer_stackD, 'u', 'd', 'o');

  // buffer allocated in heap
  charBuffer *buffer_heapU = new charBuffer;
  charBuffer *buffer_heapM = new charBuffer;
  charBuffer *buffer_heapD = new charBuffer;
  char_buffer_init(buffer_heapU, 'u', 'd', 'o');
  char_buffer_init(buffer_heapM, 'u', 'd', 'o');
  char_buffer_init(buffer_heapD, 'u', 'd', 'o');

  int store_type = argv[1][0] - '0';
  long long distance_up   = 0;
  long long distance_down = 0;

  switch(store_type) {
  case 0: // stack
    GET_DISTANCE(distance_up,   buffer_stackU.underflow, buffer_stackM.data);
    GET_DISTANCE(distance_down, buffer_stackD.overflow,  buffer_stackM.data);
    return
      read_by_pointer(buffer_stackM.data, distance_up,   7, 1, 'u') &
      read_by_pointer(buffer_stackM.data, distance_down, 7, 1, 'o') ;
  case 1: // heap
    GET_DISTANCE(distance_up,   buffer_heapU->underflow, buffer_heapM->data);
    GET_DISTANCE(distance_down, buffer_heapD->overflow,  buffer_heapM->data);
    return
      read_by_pointer(buffer_heapM->data, distance_up,   7, 1, 'u') &
      read_by_pointer(buffer_heapM->data, distance_down, 7, 1, 'o') ;
  case 2: // data
    GET_DISTANCE(distance_up,   buffer_dataU.underflow, buffer_dataM.data);
    GET_DISTANCE(distance_down, buffer_dataD.overflow,  buffer_dataM.data);
    return
      read_by_pointer(buffer_dataM.data, distance_up,   7, 1, 'u') &
      read_by_pointer(buffer_dataM.data, distance_down, 7, 1, 'o') ;
  case 3: // rodata
    GET_DISTANCE(distance_up,   buffer_rodataU.underflow, buffer_rodataM.data);
    GET_DISTANCE(distance_down, buffer_rodataD.overflow,  buffer_rodataM.data);
    return
      read_by_pointer(buffer_rodataM.data, distance_up,   7, 1, 'u') &
      read_by_pointer(buffer_rodataM.data, distance_down, 7, 1, 'o') ;
  default:
    return -1;
  }
}
