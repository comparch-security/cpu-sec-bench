#include "include/mss.hpp"
#include "include/conf.hpp"
#include <stdlib.h>
#include <string.h>

int main(int argc, char** argv){

  INIT_BUFFER;
  int flow_type  = argv[1][0] - '0';
  int rv = -1;

  if(flow_type == 0){
    rv = read_by_pointer(STR2UL(RSTR(REGION_KIND), BSTR(BUFFER_KIND)),   BUFFER_SIZE, BUFFER_SIZE-1, 1, 'o');
  }else if(flow_type == 1){
    rv = read_by_pointer(STR2UL(RSTR(REGION_KIND), BSTR(BUFFER_KIND)),  -BUFFER_SIZE, BUFFER_SIZE-1, 1, 'u');
  }else if(flow_type == 2){ //fake use to compress Msvc Optimization
    rv = read_by_pointer(STR2UL(STR2UL(RSTR(REGION_KIND), BSTR(BUFFER_KIND)) ,overflow),   BUFFER_SIZE, BUFFER_SIZE-1, 1, 'u');
  }else if(flow_type == 3){ //fake use to compress Msvc Optimization
    rv = read_by_pointer(STR2UL(STR2UL(RSTR(REGION_KIND), BSTR(BUFFER_KIND)) ,underflow),  -BUFFER_SIZE, BUFFER_SIZE-1, 1, 'u');
  }
  UNINIT_BUFFER;
  return rv;
}