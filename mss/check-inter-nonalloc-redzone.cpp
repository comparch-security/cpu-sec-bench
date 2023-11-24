#include "include/mss.hpp"
#include "include/assembly.hpp"
#include "include/conf.hpp"
#include "include/temp_file.hpp"
#include <cstdlib>

int g_argc;
char** g_argv;

FORCE_NOINLINE int check_every_aligned(char const* const* ptr) {
  long long abs_dist = 0;
  for(int i = 0; i < 16; i+=2){
    long long aligned_len = 0x1LL << (i >> 1);
    GET_DISTANCE(abs_dist, (long long)ptr[i+1], (long long)ptr[i]);
    write_to_temp_file(abs_dist, g_argc, g_argv);
    abs_dist = llabs(abs_dist);
    if(aligned_len == abs_dist){
      return 64;
    }
  }
  return 65;
}

int main(int argc, char* argv[])
{
  INIT_BUFFER;
  g_argc = argc;
  g_argv = argv;
  return check_every_aligned(STR2UL(RSTR(REGION_KIND), BSTR(BUFFER_KIND)));

  UNINIT_BUFFER;
}
