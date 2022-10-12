#include "include/mss.hpp"
#include "include/assembly.hpp"
#include <cstdlib>

const charBuffer buffer_rodata('u','d','o');
const char rodata_p0[1]     = {};const char rodata_p0_dup[1]   = {};
const char rodata_p1[2]     = {};const char rodata_p1_dup[2]   = {};
const char rodata_p2[4]     = {};const char rodata_p2_dup[4]   = {};
const char rodata_p3[8]     = {};const char rodata_p3_dup[8]   = {};
const char rodata_p4[16]    = {};const char rodata_p4_dup[16]  = {};
const char rodata_p5[32]    = {};const char rodata_p5_dup[32]  = {};
const char rodata_p6[64]    = {};const char rodata_p6_dup[64]  = {};
const char rodata_p7[128]   = {};const char rodata_p7_dup[128] = {};
const char* rodata_ptrarr[] = {rodata_p0,rodata_p0_dup,
                               rodata_p1,rodata_p1_dup,
                               rodata_p2,rodata_p2_dup,
                               rodata_p3,rodata_p3_dup,
                               rodata_p4,rodata_p4_dup,
                               rodata_p5,rodata_p5_dup,
                               rodata_p6,rodata_p6_dup,
                               rodata_p7,rodata_p7_dup};

charBuffer buffer_data('u','d','o');
const char data_p0[1]     = {};const char data_p0_dup[1]   = {};
const char data_p1[2]     = {};const char data_p1_dup[2]   = {};
const char data_p2[4]     = {};const char data_p2_dup[4]   = {};
const char data_p3[8]     = {};const char data_p3_dup[8]   = {};
const char data_p4[16]    = {};const char data_p4_dup[16]  = {};
const char data_p5[32]    = {};const char data_p5_dup[32]  = {};
const char data_p6[64]    = {};const char data_p6_dup[64]  = {};
const char data_p7[128]   = {};const char data_p7_dup[128] = {};
const char* data_ptrarr[] = {data_p0,data_p0_dup,
                             data_p1,data_p1_dup,
                             data_p2,data_p2_dup,
                             data_p3,data_p3_dup,
                             data_p4,data_p4_dup,
                             data_p5,data_p5_dup,
                             data_p6,data_p6_dup,
                             data_p7,data_p7_dup};

int check_every_aligned(char const* const* ptr){
  long long abs_dist = 0;
  for(int i = 0; i < 16; i+=2){
    long long aligned_len = 0x1LL << (i >> 1);
    GET_DISTANCE(abs_dist, ptr[i+1], ptr[i]);
    abs_dist = llabs(abs_dist);
    if(aligned_len == abs_dist){
      return 0;
    }
  }
  return -1;
}

int main(int argc, char* argv[])
{
  charBuffer buffer_stack('u','d','o');
  const char stack_p0[1]     = {};const char stack_p0_dup[1]   = {};
  const char stack_p1[2]     = {};const char stack_p1_dup[2]   = {};
  const char stack_p2[4]     = {};const char stack_p2_dup[4]   = {};
  const char stack_p3[8]     = {};const char stack_p3_dup[8]   = {};
  const char stack_p4[16]    = {};const char stack_p4_dup[16]  = {};
  const char stack_p5[32]    = {};const char stack_p5_dup[32]  = {};
  const char stack_p6[64]    = {};const char stack_p6_dup[64]  = {};
  const char stack_p7[128]   = {};const char stack_p7_dup[128] = {};
  const char* stack_ptrarr[] = {stack_p0,stack_p0_dup,
                                stack_p1,stack_p1_dup,
                                stack_p2,stack_p2_dup,
                                stack_p3,stack_p3_dup,
                                stack_p4,stack_p4_dup,
                                stack_p5,stack_p5_dup,
                                stack_p6,stack_p6_dup,
                                stack_p7,stack_p7_dup};

  int store_type = argv[1][0] - '0';

  switch(store_type) {
    case 0: return check_every_aligned(stack_ptrarr);
    case 1: return check_every_aligned(data_ptrarr);
    case 2: return check_every_aligned(rodata_ptrarr);
  }
}