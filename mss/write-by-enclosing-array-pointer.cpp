#include "include/mss.hpp"
#include "include/conf.hpp"

int main(int argc, char* argv[])
{
  INIT_BUFFER;
  int flow_type  = argv[1][0] - '0';

  int rv = -1;

  if(flow_type == 0){
    update_by_pointer(MEMBEROP(STR2UL(RSTR(REGION_KIND), BSTR(BUFFER_KIND)), data), 0, 2*BUFFER_SIZE,  1, 'c');
    rv = check(MEMBEROP(STR2UL(RSTR(REGION_KIND), BSTR(BUFFER_KIND)), overflow),      BUFFER_SIZE,  1, 'c');
  }else if(flow_type == 1){
    update_by_pointer(MEMBEROP(STR2UL(RSTR(REGION_KIND), BSTR(BUFFER_KIND)), data), 0, -(BUFFER_SIZE+1),  -1, 'c');
    rv = check(MEMBEROP(STR2UL(RSTR(REGION_KIND), BSTR(BUFFER_KIND)), underflow),      BUFFER_SIZE,  1, 'c');
  }


  UNINIT_BUFFER;
  return rv;
}
