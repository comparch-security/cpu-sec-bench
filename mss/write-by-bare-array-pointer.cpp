#include "include/mss.hpp"
#include <stdlib.h>
#include <string.h>

const char rodata_underflow[8] = {'u','u','u','u','u','u','u',0};
const char rodata_data[8]      = {'d','d','d','d','d','d','d',0};
const char rodata_overflow[8]  = {'o','o','o','o','o','o','o',0};

char data_underflow[8]  = {'u','u','u','u','u','u','u',0};
char data_data[8]       = {'d','d','d','d','d','d','d',0};
char data_overflow[8]   = {'o','o','o','o','o','o','o',0};

int main(int argc, char** argv){

    char stack_underflow[8] = {'u','u','u','u','u','u','u',0};
    char stack_data[8]      = {'d','d','d','d','d','d','d',0};
    char stack_overflow[8]  = {'o','o','o','o','o','o','o',0};

    char* heap_underflow = (char*) malloc(sizeof(char)* 8);
    char* heap_data      = (char*) malloc(sizeof(char)* 8);
    char* heap_overflow  = (char*) malloc(sizeof(char)* 8);

    memset(heap_underflow,'u',8 /(sizeof(int) / sizeof(char)));
    memset(heap_data,'u',8 /(sizeof(int) / sizeof(char)));
    memset(heap_overflow,'u',8 /(sizeof(int) / sizeof(char)));

    heap_underflow[7] = 0;
    heap_data[7]      = 0;
    heap_overflow[7]  = 0;

  int store_type = argv[1][0] - '0';
  int flow_type  = argv[2][0] - '0';

  int rv;


  switch(store_type*2+flow_type) {
  case 0: // stack overflow
    update_by_pointer(stack_data, 0, 16,  1, 'c');
    rv = check(stack_overflow,    8,  1, 'c');
    break;
  case 1: // stack underflow
    update_by_pointer(stack_data, 0, -9, -1, 'c');
    rv = check(stack_underflow,   8,  1, 'c');
    break;
  case 2: // heap overflow
    update_by_pointer(heap_data, 0, 16,  1, 'c');
    rv = check(heap_overflow,    8,  1, 'c');
    break;
  case 3: // heap underflow
    update_by_pointer(heap_data, 0, -9, -1, 'c');
    rv = check(heap_underflow,   8,  1, 'c');
    break;
  case 4: // data overflow
    update_by_pointer(data_data,  0, 16,  1, 'c');
    rv = check(data_overflow,       8,  1, 'c');
    break;
  case 5: // data underflow
    update_by_pointer(data_data,  0, -9, -1, 'c');
    rv = check(data_underflow,      8,  1, 'c');
    break;
  default:
    rv = -1;
  }

  free(heap_underflow);
  free(heap_data);
  free(heap_overflow);
  return rv;
}