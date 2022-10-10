#include "include/mss.hpp"
#include "include/assembly.hpp"
#include <cstdlib>

const charBuffer buffer_rodata('u','d','o');
const char rodata_p0[1]   = {};
const char rodata_p1[1]   = {};
const char rodata_p2[2]   = {};
const char rodata_p3[4]   = {};
const char rodata_p4[8]   = {};
const char rodata_p5[16]  = {};
const char rodata_p6[32]  = {};
const char rodata_p7[64]  = {};
const char rodata_p8[128] = {};
const char* rodata_ptrarr[] = {rodata_p0,rodata_p1,rodata_p2,rodata_p3,rodata_p4,rodata_p5,rodata_p6,rodata_p7,rodata_p8};

charBuffer buffer_data('u','d','o');
char data_p0[1] = {};
char data_p1[1] = {};
char data_p2[2] = {};
char data_p3[4] = {};
char data_p4[8] = {};
char data_p5[16]= {};
char data_p6[32]= {};
char data_p7[64]= {};
char data_p8[128]={};
char* data_ptrarr[] = {data_p0,data_p1,data_p2,data_p3,data_p4,data_p5,data_p6,data_p7,data_p8};

int check_every_aligned(char const* const* ptr, long long abs_len){
  for(int i = 0; i <= 7; i++){
    long long aligned_len = round_up(sizeof(data_p0),0x1LL << i);
    if(aligned_len == abs_len && aligned_len == llabs(ptr[i+1] - ptr[i])){
      return 0;
    }
  }
  return -1;
}

int main(int argc, char* argv[])
{
  charBuffer buffer_stack('u','d','o');
  char stack_p0[1] = {};
  char stack_p1[1] = {};
  char stack_p2[2] = {};
  char stack_p3[4] = {};
  char stack_p4[8] = {};
  char stack_p5[16]= {};
  char stack_p6[32]= {};
  char stack_p7[64]= {};
  char stack_p8[128]={};
  char* stack_ptrarr[] = {stack_p0,stack_p1,stack_p2,stack_p3,stack_p4,stack_p5,stack_p6,stack_p7,stack_p8};

  int store_type = argv[1][0] - '0';

  long long length = 0;

  switch(store_type) {
    case 0: GET_DISTANCE(length, &stack_p1, &stack_p0);
            return check_every_aligned(stack_ptrarr,llabs(length));
    case 1: GET_DISTANCE(length, &data_p1, &data_p0);
            return check_every_aligned(data_ptrarr,llabs(length));
    case 2: GET_DISTANCE(length, &rodata_p1, &rodata_p0);
            return check_every_aligned(rodata_ptrarr,llabs(length));
    case 3: GET_DISTANCE(length, buffer_stack.data, buffer_stack.underflow); break;
    case 4: GET_DISTANCE(length, buffer_data.data, buffer_data.underflow); break;
    case 5: GET_DISTANCE(length, buffer_rodata.data, buffer_rodata.underflow); break;
  }
  long long abs_len = llabs(length);
  if(abs_len == CB_BUF_LEN) return 0;

  return -1;
}