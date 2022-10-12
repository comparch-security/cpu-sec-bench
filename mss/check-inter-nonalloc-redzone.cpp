#include "include/mss.hpp"
#include "include/assembly.hpp"
#include <cstdlib>

const char rodata_p00[1],   rodata_p01[1];
const char rodata_p10[2],   rodata_p11[2];
const char rodata_p20[4],   rodata_p21[4];
const char rodata_p30[8],   rodata_p31[8];
const char rodata_p40[16],  rodata_p41[16];
const char rodata_p50[32],  rodata_p51[32];
const char rodata_p60[64],  rodata_p61[64];
const char rodata_p70[128], rodata_p71[128];
const char* rodata_ptrarr[] = {
  rodata_p00, rodata_p01, rodata_p10, rodata_p11,
  rodata_p20, rodata_p21, rodata_p30, rodata_p31,
  rodata_p40, rodata_p41, rodata_p50, rodata_p51,
  rodata_p60, rodata_p61, rodata_p70, rodata_p71};

char data_p00[1],   data_p01[1];
char data_p10[2],   data_p11[2];
char data_p20[4],   data_p21[4];
char data_p30[8],   data_p31[8];
char data_p40[16],  data_p41[16];
char data_p50[32],  data_p51[32];
char data_p60[64],  data_p61[64];
char data_p70[128], data_p71[128];
char* rodata_ptrarr[] = {
  data_p00, data_p01, data_p10, data_p11,
  data_p20, data_p21, data_p30, data_p31,
  data_p40, data_p41, data_p50, data_p51,
  data_p60, data_p61, data_p70, data_p71};

int check_every_aligned(char const* const* ptr) {
  long long abs_dist = 0;
  for(int i = 0; i < 16; i+=2){
    long long aligned_len = 0x1LL << (i >> 1);
    GET_DISTANCE(abs_dist, ptr[i+1], ptr[i]);
    abs_dist = llabs(abs_dist);
    if(aligned_len == abs_dist){
      return 0;
    }
  }
  return 1;
}

int main(int argc, char* argv[])
{
  char stack_p00[1],   stack_p01[1];
  char stack_p10[2],   stack_p11[2];
  char stack_p20[4],   stack_p21[4];
  char stack_p30[8],   stack_p31[8];
  char stack_p40[16],  stack_p41[16];
  char stack_p50[32],  stack_p51[32];
  char stack_p60[64],  stack_p61[64];
  char stack_p70[128], stack_p71[128];
  char* stack_ptrarr[] = {
    stack_p00, stack_p01, stack_p10, stack_p11,
    stack_p20, stack_p21, stack_p30, stack_p31,
    stack_p40, stack_p41, stack_p50, stack_p51,
    stack_p60, stack_p61, stack_p70, stack_p71};

  int store_type = argv[1][0] - '0';

  switch(store_type) {
    case 0: return check_every_aligned(stack_ptrarr);
    case 1: return check_every_aligned(data_ptrarr);
    case 2: return check_every_aligned(rodata_ptrarr);
  }
}
