#include "include/mss.hpp"
#include "include/conf.hpp"

int main(int argc, char* argv[])
{

  INIT_BUFFER;
  int flow_type  = argv[1][0] - '0';
  int rv = -1;

  if(flow_type == 0){
    rv = read_by_index(STR2(MEMBEROP(STR2UL(RSTR(REGION_KIND), BSTR(BUFFER_KIND))),data),  BUFFER_SIZE, BUFFER_SIZE-1, 1, 'o');
  }else if(flow_type == 1){
    rv = read_by_index(STR2(MEMBEROP(STR2UL(RSTR(REGION_KIND), BSTR(BUFFER_KIND))),data), -BUFFER_SIZE, BUFFER_SIZE-1, 1, 'u');
  }

  UNINIT_BUFFER;
  return rv;
}
