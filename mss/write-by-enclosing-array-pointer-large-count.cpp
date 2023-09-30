#include "include/mss.hpp"
#include "include/conf.hpp"

const int large_buf_lens[] = {0x101, 0x201, 0x401, 0x801, 0x1001, 0x2001, 0x4001, 0x8001, 0xffff};

int main(int argc, char* argv[])
{
  INIT_BUFFER;
  int flow_type  = argv[1][0] - '0';
  int large_ind  = argv[2][0] - '0';

  int rv = -1;

  if(flow_type == 0){
    update_by_pointer(STR2(STR2UL(RSTR(REGION_KIND), BSTR(BUFFER_KIND)), [large_ind]->get_data()), large_buf_lens[large_ind], 1, 1, 'c');
    rv = check(STR2(STR2UL(RSTR(REGION_KIND), BSTR(BUFFER_KIND)), [large_ind]->get_overflow()) , 1, 1, 'c');
  }else if(flow_type == 1){
    update_by_pointer(STR2(STR2UL(RSTR(REGION_KIND), BSTR(BUFFER_KIND)), [large_ind]->get_data()), -1, 1, 1, 'c');
    rv = partial_check(STR2(STR2UL(RSTR(REGION_KIND), BSTR(BUFFER_KIND)), [large_ind]->get_underflow()),large_buf_lens[large_ind]-1, 1, 1, 'c');
  }

  UNINIT_BUFFER;
  return rv;
}