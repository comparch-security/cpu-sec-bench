#include "include/mss.hpp"
#include "include/assembly.hpp"
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
  int flow_type = argv[1][0] - '0';
  long long distance_up   = 0;
  long long distance_down = 0;

  switch(store_type) {
  case 0: { // stack
    GET_DISTANCE(distance_up,   buffer_stackU.data, buffer_stackM.data);
    GET_DISTANCE(distance_down, buffer_stackD.data, buffer_stackM.data);
    char *buf          = (distance_up > 0) ^ (flow_type == 1) ? buffer_stackU.data : buffer_stackD.data;
    long long distance = (distance_up > 0) ^ (flow_type == 1) ? distance_up        : distance_down;
    update_by_pointer(buffer_stackM.data, distance,   8, 1, 'c');
    return check(buf,  8,  1, 'c');
  }
  case 1: { // heap
    GET_DISTANCE(distance_up,   buffer_heapU->data, buffer_heapM->data);
    GET_DISTANCE(distance_down, buffer_heapD->data, buffer_heapM->data);
    char *buf          = (distance_up > 0) ^ (flow_type == 1) ? buffer_heapU->data : buffer_heapD->data;
    long long distance = (distance_up > 0) ^ (flow_type == 1) ? distance_up        : distance_down;
    update_by_pointer(buffer_heapM->data, distance,   8, 1, 'c');
    return check(buf,  8,  1, 'c');
  }
  case 2: { // data
    GET_DISTANCE(distance_up,   buffer_dataU.data, buffer_dataM.data);
    GET_DISTANCE(distance_down, buffer_dataD.data, buffer_dataM.data);
    char *buf          = (distance_up > 0) ^ (flow_type == 1) ? buffer_dataU.data : buffer_dataD.data;
    long long distance = (distance_up > 0) ^ (flow_type == 1) ? distance_up       : distance_down;
    update_by_pointer(buffer_dataM.data, distance,   8, 1, 'c');
    return check(buf,  8,  1, 'c');
  }
  default:
    return -1;
  }
}
