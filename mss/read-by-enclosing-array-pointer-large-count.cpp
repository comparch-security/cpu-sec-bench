#include "include/mss.hpp"
#include "include/conf.hpp"
//This test is used to find vlunerblity of compressed capability

const int large_buf_lens[] = {0x101, 0x201, 0x401, 0x801, 0x1001, 0x2001, 0x4001, 0x8001};

int main(int argc, char* argv[])
{
  INIT_BUFFER;
  int flow_type  = argv[1][0] - '0';
  int large_ind  = argv[2][0] - '0';

  int rv = -1;

  if(flow_type == 0){
    rv = read_by_pointer(ARROW(STR2BR(STR2UL(RSTR(REGION_KIND), BSTR(BUFFER_KIND)), large_ind),get_const_data()), large_buf_lens[large_ind], 1, 1, 'o');
  }else if(flow_type == 1){
    rv = read_by_pointer(ARROW(STR2BR(STR2UL(RSTR(REGION_KIND), BSTR(BUFFER_KIND)), large_ind),get_const_data()), -2, 1, 1, 'u');
  }
  UNINIT_BUFFER;
  return rv;
}
