#include "include/mss.hpp"
#include "include/conf.hpp"
#include "include/assembly.hpp"

int main(int argc, char* argv[])
{

  INIT_BUFFER;
  int flow_type  = argv[1][0] - '0';
  int rv = -1;

  if(flow_type == 0){
    rv = read_by_index(MEMBEROP(STR2UL(RSTR(REGION_KIND), BSTR(BUFFER_KIND)),data),  BUFFER_SIZE, BUFFER_SIZE-1, 1, 'o');
  }else if(flow_type == 1){
    rv = read_by_index(MEMBEROP(STR2UL(RSTR(REGION_KIND), BSTR(BUFFER_KIND)),data), -BUFFER_SIZE, BUFFER_SIZE-1, 1, 'u');
  }else if(flow_type >= 2 && flow_type < 4){
    long long assem_dist;
    long long src = (long long)MEMBEROP(STR2UL(RSTR(REGION_KIND), BSTR(BUFFER_KIND)),data);
    if (flow_type == 2){
      long long dst = (long long)MEMBEROP(STR2UL(RSTR(REGION_KIND), BSTR(BUFFER_KIND)),overflow);
      GET_DISTANCE(assem_dist, dst, src);
      rv = read_by_index(MEMBEROP(STR2UL(RSTR(REGION_KIND), BSTR(BUFFER_KIND)),data),  assem_dist, BUFFER_SIZE-1, 1, 'o');
    }else if(flow_type == 3){
      long long dst = (long long)MEMBEROP(STR2UL(RSTR(REGION_KIND), BSTR(BUFFER_KIND)),underflow);
      GET_DISTANCE(assem_dist, dst, src);
      rv = read_by_index(MEMBEROP(STR2UL(RSTR(REGION_KIND), BSTR(BUFFER_KIND)),data), assem_dist, BUFFER_SIZE-1, 1, 'u');
    }
  }else if(flow_type >= 4){
    int othersrc_dist = argv[2][0] - '0';
    if(flow_type == 4){
      rv = read_by_index(MEMBEROP(STR2UL(RSTR(REGION_KIND), BSTR(BUFFER_KIND)),data), othersrc_dist, BUFFER_SIZE-1, 1, 'o');
    }else if(flow_type == 5){
      rv = read_by_index(MEMBEROP(STR2UL(RSTR(REGION_KIND), BSTR(BUFFER_KIND)),data), othersrc_dist, BUFFER_SIZE-1, 1, 'u');
    }
  }

  UNINIT_BUFFER;
  return rv;
}
