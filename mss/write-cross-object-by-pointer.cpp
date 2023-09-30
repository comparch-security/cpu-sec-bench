#include "include/mss.hpp"
#include "include/assembly.hpp"
#include "include/conf.hpp"

int main(int argc, char* argv[])
{
  INIT_BUFFER;
  int flow_type = argv[1][0] - '0';
  long long distance_up   = 0;
  long long distance_down = 0;

  int rv = -1;

  GET_DISTANCE(distance_up, (long long)MEMBEROP(STR2UL(RSTR(REGION_KIND), BSTR(BUFFER_KIND))_underflow)data, (long long)MEMBEROP(STR2UL(RSTR(REGION_KIND), BSTR(BUFFER_KIND)))data);
  GET_DISTANCE(distance_down, (long long)MEMBEROP(STR2UL(RSTR(REGION_KIND), BSTR(BUFFER_KIND))_overflow)data, (long long)MEMBEROP(STR2UL(RSTR(REGION_KIND), BSTR(BUFFER_KIND)))data);
  char *buf = (distance_up > 0) ^ (flow_type == 1) ? STR2(MEMBEROP(STR2UL(STR2UL(RSTR(REGION_KIND), BSTR(BUFFER_KIND)), underflow)), data) : 
                                                     STR2(MEMBEROP(STR2UL(STR2UL(RSTR(REGION_KIND), BSTR(BUFFER_KIND)), overflow)), data);
  long long distance = (distance_up > 0) ^ (flow_type == 1) ? distance_up        : distance_down;
  update_by_pointer(STR2(MEMBEROP(STR2UL(RSTR(REGION_KIND), BSTR(BUFFER_KIND))), data), distance,   BUFFER_SIZE, 1, 'c');
  rv = check(buf,  BUFFER_SIZE,  1, 'c');

  UNINIT_BUFFER;
  return rv;
}
