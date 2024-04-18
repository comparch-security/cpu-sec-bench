#include "include/mss.hpp"
#include "include/conf.hpp"
#include "include/assembly.hpp"
#include <stdlib.h>
#include <string.h>

int main(int argc, char** argv){

  INIT_BUFFER;

  int flow_type  = argv[1][0] - '0';

  int rv = -1;

  if(flow_type == 0){
    update_by_index(HEAPTR_TO_ARRAYREF(STR2UL(RSTR(REGION_KIND), BSTR(BUFFER_KIND))), 0, 2*BUFFER_SIZE,  1, 'c');
    rv = check(STR2UL(STR2UL(RSTR(REGION_KIND), BSTR(BUFFER_KIND)) ,overflow),      BUFFER_SIZE,  1, 'c');
  }else if(flow_type == 1){
    update_by_index(HEAPTR_TO_ARRAYREF(STR2UL(RSTR(REGION_KIND), BSTR(BUFFER_KIND))), 0, -(BUFFER_SIZE+1),  -1, 'c');
    rv = check(STR2UL(STR2UL(RSTR(REGION_KIND), BSTR(BUFFER_KIND)) ,underflow),      BUFFER_SIZE,  1, 'c');
  }else if(flow_type >= 2 && flow_type < 4){
    long long assem_dist;
    long long src = (long long)STR2UL(RSTR(REGION_KIND), BSTR(BUFFER_KIND));
    if (flow_type == 2){
      long long dst = (long long)STR2UL(STR2UL(RSTR(REGION_KIND), BSTR(BUFFER_KIND)) ,overflow);
      GET_DISTANCE(assem_dist, dst, src);
      update_by_index(HEAPTR_TO_ARRAYREF(STR2UL(RSTR(REGION_KIND), BSTR(BUFFER_KIND))), assem_dist, BUFFER_SIZE,  1, 'c');
      rv = check(STR2UL(STR2UL(RSTR(REGION_KIND), BSTR(BUFFER_KIND)) ,overflow),      BUFFER_SIZE,  1, 'c');
    }else if(flow_type == 3){
      long long dst = (long long)STR2UL(STR2UL(RSTR(REGION_KIND), BSTR(BUFFER_KIND)) ,underflow);
      GET_DISTANCE(assem_dist, dst, src);
      update_by_index(HEAPTR_TO_ARRAYREF(STR2UL(RSTR(REGION_KIND), BSTR(BUFFER_KIND))), assem_dist, BUFFER_SIZE,  1, 'c');
      rv = check(STR2UL(STR2UL(RSTR(REGION_KIND), BSTR(BUFFER_KIND)) ,underflow),      BUFFER_SIZE,  1, 'c');
    }
  }else if(flow_type >= 4){
    int othersrc_dist = argv[2][0] - '0';
    if(flow_type == 4){
      update_by_index(HEAPTR_TO_ARRAYREF(STR2UL(RSTR(REGION_KIND), BSTR(BUFFER_KIND))), othersrc_dist, BUFFER_SIZE,  1, 'c');
      rv = check(STR2UL(STR2UL(RSTR(REGION_KIND), BSTR(BUFFER_KIND)) ,overflow),      BUFFER_SIZE,  1, 'c');
    }else if(flow_type == 5){
      update_by_index(HEAPTR_TO_ARRAYREF(STR2UL(RSTR(REGION_KIND), BSTR(BUFFER_KIND))), othersrc_dist, BUFFER_SIZE,  1, 'c');
      rv = check(STR2UL(STR2UL(RSTR(REGION_KIND), BSTR(BUFFER_KIND)) ,underflow),      BUFFER_SIZE,  1, 'c');
    }
  }

  UNINIT_BUFFER;
  return rv;
}