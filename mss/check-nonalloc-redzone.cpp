#include "include/mss.hpp"
#include "include/assembly.hpp"
#include <cstdlib>
#include <iostream>

/* For global/stack mem type,
 * gcc asan align granularity is fixed at 32 bytes.
 */
#define NONALLOCBUFFER_ALIGN_GRANULARITY 32

const charBuffer buffer_rodata('u','d','o');
const charBuffer buffer_rodata_dup('u','d','o');

charBuffer buffer_data('u','d','o');
charBuffer buffer_data_dup('u','d','o');

int main(int argc, char* argv[])
{

  charBuffer buffer_stack('u','d','o');
  charBuffer buffer_stack_dup('u','d','o');

  int store_type = argv[1][0] - '0';

  long long length = 0;

  switch(store_type) {
    case 0: GET_DISTANCE(length, &buffer_stack, &buffer_stack_dup);break;
    case 1: GET_DISTANCE(length, &buffer_data, &buffer_data_dup); break;
    case 2: GET_DISTANCE(length, &buffer_rodata, &buffer_rodata_dup); break;
    case 3: GET_DISTANCE(length, buffer_stack.data, buffer_stack.underflow); break;
    case 4: GET_DISTANCE(length, buffer_data.data, buffer_data.underflow); break;
    case 5: GET_DISTANCE(length, buffer_rodata.data, buffer_rodata.underflow); break;
  }

  long long target = store_type < 3 ? 
  round_up(sizeof(charBuffer),NONALLOCBUFFER_ALIGN_GRANULARITY) : 
  round_up(CB_BUF_LEN,1);

  if(target != llabs(length)){
    /* current mem type has redzone */
    return -1;
  }else
    return 0;
}