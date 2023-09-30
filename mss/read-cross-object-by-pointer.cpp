#include "include/mss.hpp"
#include "include/assembly.hpp"
#include "include/conf.hpp"

int main(int argc, char* argv[])
{
  INIT_BUFFER;
  long long distance_up   = 0;
  long long distance_down = 0;

  int rv = 2;
    GET_DISTANCE(distance_up, (long long)MEMBEROP(STR2UL(RSTR(REGION_KIND), BSTR(BUFFER_KIND))_underflow)underflow, (long long)MEMBEROP(STR2UL(RSTR(REGION_KIND), BSTR(BUFFER_KIND)))data);
    GET_DISTANCE(distance_down, (long long)MEMBEROP(STR2UL(RSTR(REGION_KIND), BSTR(BUFFER_KIND))_overflow)overflow, (long long)MEMBEROP(STR2UL(RSTR(REGION_KIND), BSTR(BUFFER_KIND)))data);
    
    rv =
      read_by_pointer(STR2(MEMBEROP(STR2UL(RSTR(REGION_KIND), BSTR(BUFFER_KIND))),data), distance_up,   BUFFER_SIZE-1, 1, 'u') &
      read_by_pointer(STR2(MEMBEROP(STR2UL(RSTR(REGION_KIND), BSTR(BUFFER_KIND))),data), distance_down, BUFFER_SIZE-1, 1, 'o') ;


  UNINIT_BUFFER;
  return rv;
}
