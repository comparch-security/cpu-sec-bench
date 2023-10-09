#include "include/mss.hpp"
#include "include/conf.hpp"
#include "include/assembly.hpp"
#include <cstdlib>

int main(int argc, char* argv[])
{
  INIT_BUFFER;
  long long distance = 0;

  GET_DISTANCE(distance, (long long)MEMBEROP(STR2(STR2UL(RSTR(REGION_KIND), BSTR(BUFFER_KIND)), _overflow),data),(long long)MEMBEROP(STR2UL(RSTR(REGION_KIND), BSTR(BUFFER_KIND)),data));
  if(distance > 0) {
    update_by_pointer(MEMBEROP(STR2UL(RSTR(REGION_KIND), BSTR(BUFFER_KIND)),data), 0, distance+BUFFER_SIZE, 1, 'c');
    exit(check(MEMBEROP(STR2UL(STR2UL(RSTR(REGION_KIND), BSTR(BUFFER_KIND)),overflow),data),  BUFFER_SIZE,  1, 'c'));
  } else {
    update_by_pointer(MEMBEROP(STR2UL(STR2UL(RSTR(REGION_KIND), BSTR(BUFFER_KIND)),overflow),data), 0, -distance+BUFFER_SIZE, 1, 'c');
    exit(check(MEMBEROP(STR2UL(RSTR(REGION_KIND), BSTR(BUFFER_KIND)),data),  BUFFER_SIZE,  1, 'c'));
  }
  UNINIT_BUFFER;
  exit(2);
}
