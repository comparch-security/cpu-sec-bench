#include "include/mss.hpp"
//This test is used to find vlunerblity of compressed capability
const LargeMemberBuffer1 buffer_rodata1('u','d','o');
const LargeMemberBuffer2 buffer_rodata2('u','d','o');
const LargeMemberBuffer3 buffer_rodata3('u','d','o');
const LargeMemberBuffer4 buffer_rodata4('u','d','o');
const LargeMemberBuffer5 buffer_rodata5('u','d','o');
const LargeMemberBuffer6 buffer_rodata6('u','d','o');
const LargeMemberBuffer7 buffer_rodata7('u','d','o');
const LargeMemberBuffer8 buffer_rodata8('u','d','o');
// buffer in data
LargeMemberBuffer1 buffer_data1('u','d','o');
LargeMemberBuffer2 buffer_data2('u','d','o');
LargeMemberBuffer3 buffer_data3('u','d','o');
LargeMemberBuffer4 buffer_data4('u','d','o');
LargeMemberBuffer5 buffer_data5('u','d','o');
LargeMemberBuffer6 buffer_data6('u','d','o');
LargeMemberBuffer7 buffer_data7('u','d','o');
LargeMemberBuffer8 buffer_data8('u','d','o');

int main(int argc, char* argv[])
{
  // buffer in local stack
  LargeMemberBuffer1 buffer_stack1('u','d','o');
  LargeMemberBuffer2 buffer_stack2('u','d','o');
  LargeMemberBuffer3 buffer_stack3('u','d','o');
  LargeMemberBuffer4 buffer_stack4('u','d','o');
  LargeMemberBuffer5 buffer_stack5('u','d','o');
  LargeMemberBuffer6 buffer_stack6('u','d','o');
  LargeMemberBuffer7 buffer_stack7('u','d','o');
  LargeMemberBuffer8 buffer_stack8('u','d','o');

  // buffer allocated in heap
  LargeMemberBuffer1 *buffer_heap1 = new LargeMemberBuffer1('u','d','o');
  LargeMemberBuffer2 *buffer_heap2 = new LargeMemberBuffer2('u','d','o');
  LargeMemberBuffer3 *buffer_heap3 = new LargeMemberBuffer3('u','d','o');
  LargeMemberBuffer4 *buffer_heap4 = new LargeMemberBuffer4('u','d','o');
  LargeMemberBuffer5 *buffer_heap5 = new LargeMemberBuffer5('u','d','o');
  LargeMemberBuffer6 *buffer_heap6 = new LargeMemberBuffer6('u','d','o');
  LargeMemberBuffer7 *buffer_heap7 = new LargeMemberBuffer7('u','d','o');
  LargeMemberBuffer8 *buffer_heap8 = new LargeMemberBuffer8('u','d','o');

  int store_type = argv[1][0] - '0';
  int flow_type  = argv[2][0] - '0';
  int large_ind  = argv[3][0] - '0';

  int rv;

  switch(store_type*2+flow_type) {
  case 0: // stack overflow
    if(large_ind == 1){
      update_by_pointer(buffer_stack1.data,   LARGE_UNALIGNED_BUF_LEN1, 1, 1, 'c');
      rv = check(buffer_stack1.overflow,  1,  1,  'c');
    }else if(large_ind == 2){
      update_by_pointer(buffer_stack2.data,   LARGE_UNALIGNED_BUF_LEN2, 1, 1, 'c');
      rv = check(buffer_stack2.overflow,  1,  1,  'c');
    }else if(large_ind == 3){
      update_by_pointer(buffer_stack3.data,   LARGE_UNALIGNED_BUF_LEN3, 1, 1, 'c');
      rv = check(buffer_stack3.overflow,  1,  1,  'c');
    }else if(large_ind == 4){
      update_by_pointer(buffer_stack4.data,   LARGE_UNALIGNED_BUF_LEN4, 1, 1, 'c');
      rv = check(buffer_stack4.overflow,  1,  1,  'c');
    }else if(large_ind == 5){
      update_by_pointer(buffer_stack5.data,   LARGE_UNALIGNED_BUF_LEN5, 1, 1, 'c');
      rv = check(buffer_stack5.overflow,  1,  1,  'c');
    }else if(large_ind == 6){
      update_by_pointer(buffer_stack6.data,   LARGE_UNALIGNED_BUF_LEN6, 1, 1, 'c');
      rv = check(buffer_stack6.overflow,  1,  1,  'c');
    }else if(large_ind == 7){
      update_by_pointer(buffer_stack7.data,   LARGE_UNALIGNED_BUF_LEN7, 1, 1, 'c');
      rv = check(buffer_stack7.overflow,  1,  1,  'c');
    }else if(large_ind == 8){
      update_by_pointer(buffer_stack8.data,   LARGE_UNALIGNED_BUF_LEN8, 1, 1, 'c');
      rv = check(buffer_stack8.overflow,  1,  1,  'c');
    }
    break;
  case 1: // stack underflow
    if(large_ind == 1){
      update_by_pointer(buffer_stack1.data,  -1,  1,  1,  'c');
      rv = partial_check(buffer_stack1.underflow,  LARGE_UNALIGNED_BUF_LEN1-1,  1,  1,  'c');
    }else if(large_ind == 2){
      update_by_pointer(buffer_stack2.data,  -1,  1,  1,  'c');
      rv = partial_check(buffer_stack2.underflow,  LARGE_UNALIGNED_BUF_LEN2-1,  1,  1,  'c');
    }else if(large_ind == 3){
      update_by_pointer(buffer_stack3.data,  -1,  1,  1,  'c');
      rv = partial_check(buffer_stack3.underflow,  LARGE_UNALIGNED_BUF_LEN3-1,  1,  1,  'c');
    }else if(large_ind == 4){
      update_by_pointer(buffer_stack4.data,  -1,  1,  1,  'c');
      rv = partial_check(buffer_stack4.underflow,  LARGE_UNALIGNED_BUF_LEN4-1,  1,  1,  'c');
    }else if(large_ind == 5){
      update_by_pointer(buffer_stack5.data,  -1,  1,  1,  'c');
      rv = partial_check(buffer_stack5.underflow,  LARGE_UNALIGNED_BUF_LEN5-1,  1,  1,  'c');
    }else if(large_ind == 6){
      update_by_pointer(buffer_stack6.data,  -1,  1,  1,  'c');
      rv = partial_check(buffer_stack6.underflow,  LARGE_UNALIGNED_BUF_LEN6-1,  1,  1,  'c');
    }else if(large_ind == 7){
      update_by_pointer(buffer_stack7.data,  -1,  1,  1,  'c');
      rv = partial_check(buffer_stack7.underflow,  LARGE_UNALIGNED_BUF_LEN7-1,  1,  1,  'c');
    }else if(large_ind == 8){
      update_by_pointer(buffer_stack8.data,  -1,  1,  1,  'c');
      rv = partial_check(buffer_stack8.underflow,  LARGE_UNALIGNED_BUF_LEN8-1,  1,  1,  'c');
    }
    break;
  case 2: // heap overflow
    if(large_ind == 1){
      update_by_pointer(buffer_heap1->data,   LARGE_UNALIGNED_BUF_LEN1, 1, 1, 'c');
      rv = check(buffer_heap1->overflow,  1,  1,  'c');
    }else if(large_ind == 2){
      update_by_pointer(buffer_heap2->data,   LARGE_UNALIGNED_BUF_LEN2, 1, 1, 'c');
      rv = check(buffer_heap2->overflow,  1,  1,  'c');
    }else if(large_ind == 3){
      update_by_pointer(buffer_heap3->data,   LARGE_UNALIGNED_BUF_LEN3, 1, 1, 'c');
      rv = check(buffer_heap3->overflow,  1,  1,  'c');
    }else if(large_ind == 4){
      update_by_pointer(buffer_heap4->data,   LARGE_UNALIGNED_BUF_LEN4, 1, 1, 'c');
      rv = check(buffer_heap4->overflow,  1,  1,  'c');
    }else if(large_ind == 5){
      update_by_pointer(buffer_heap5->data,   LARGE_UNALIGNED_BUF_LEN5, 1, 1, 'c');
      rv = check(buffer_heap5->overflow,  1,  1,  'c');
    }else if(large_ind == 6){
      update_by_pointer(buffer_heap6->data,   LARGE_UNALIGNED_BUF_LEN6, 1, 1, 'c');
      rv = check(buffer_heap6->overflow,  1,  1,  'c');
    }else if(large_ind == 7){
      update_by_pointer(buffer_heap7->data,   LARGE_UNALIGNED_BUF_LEN7, 1, 1, 'c');
      rv = check(buffer_heap7->overflow,  1,  1,  'c');
    }else if(large_ind == 8){
      update_by_pointer(buffer_heap8->data,   LARGE_UNALIGNED_BUF_LEN8, 1, 1, 'c');
      rv = check(buffer_heap8->overflow,  1,  1,  'c');
    }
    break;
  case 3: // heap underflow
    if(large_ind == 1){
      update_by_pointer(buffer_heap1->data,  -1,  1,  1,  'c');
      rv = partial_check(buffer_heap1->underflow,  LARGE_UNALIGNED_BUF_LEN1-1,  1,  1,  'c');
    }else if(large_ind == 2){
      update_by_pointer(buffer_heap2->data,  -1,  1,  1,  'c');
      rv = partial_check(buffer_heap2->underflow,  LARGE_UNALIGNED_BUF_LEN2-1,  1,  1,  'c');
    }else if(large_ind == 3){
      update_by_pointer(buffer_heap3->data,  -1,  1,  1,  'c');
      rv = partial_check(buffer_heap3->underflow,  LARGE_UNALIGNED_BUF_LEN3-1,  1,  1,  'c');
    }else if(large_ind == 4){
      update_by_pointer(buffer_heap4->data,  -1,  1,  1,  'c');
      rv = partial_check(buffer_heap4->underflow,  LARGE_UNALIGNED_BUF_LEN4-1,  1,  1,  'c');
    }else if(large_ind == 5){
      update_by_pointer(buffer_heap5->data,  -1,  1,  1,  'c');
      rv = partial_check(buffer_heap5->underflow,  LARGE_UNALIGNED_BUF_LEN5-1,  1,  1,  'c');
    }else if(large_ind == 6){
      update_by_pointer(buffer_heap6->data,  -1,  1,  1,  'c');
      rv = partial_check(buffer_heap6->underflow,  LARGE_UNALIGNED_BUF_LEN6-1,  1,  1,  'c');
    }else if(large_ind == 7){
      update_by_pointer(buffer_heap7->data,  -1,  1,  1,  'c');
      rv = partial_check(buffer_heap7->underflow,  LARGE_UNALIGNED_BUF_LEN7-1,  1,  1,  'c');
    }else if(large_ind == 8){
      update_by_pointer(buffer_heap8->data,  -1,  1,  1,  'c');
      rv = partial_check(buffer_heap8->underflow,  LARGE_UNALIGNED_BUF_LEN8-1,  1,  1,  'c');
    }
    break;
  case 4: // data overflow
    if(large_ind == 1){
      update_by_pointer(buffer_data1.data,   LARGE_UNALIGNED_BUF_LEN1, 1, 1, 'c');
      rv = check(buffer_data1.overflow,  1,  1,  'c');
    }else if(large_ind == 2){
      update_by_pointer(buffer_data2.data,   LARGE_UNALIGNED_BUF_LEN2, 1, 1, 'c');
      rv = check(buffer_data2.overflow,  1,  1,  'c');
    }else if(large_ind == 3){
      update_by_pointer(buffer_data3.data,   LARGE_UNALIGNED_BUF_LEN3, 1, 1, 'c');
      rv = check(buffer_data3.overflow,  1,  1,  'c');
    }else if(large_ind == 4){
      update_by_pointer(buffer_data4.data,   LARGE_UNALIGNED_BUF_LEN4, 1, 1, 'c');
      rv = check(buffer_data4.overflow,  1,  1,  'c');
    }else if(large_ind == 5){
      update_by_pointer(buffer_data5.data,   LARGE_UNALIGNED_BUF_LEN5, 1, 1, 'c');
      rv = check(buffer_data5.overflow,  1,  1,  'c');
    }else if(large_ind == 6){
      update_by_pointer(buffer_data6.data,   LARGE_UNALIGNED_BUF_LEN6, 1, 1, 'c');
      rv = check(buffer_data6.overflow,  1,  1,  'c');
    }else if(large_ind == 7){
      update_by_pointer(buffer_data7.data,   LARGE_UNALIGNED_BUF_LEN7, 1, 1, 'c');
      rv = check(buffer_data7.overflow,  1,  1,  'c');
    }else if(large_ind == 8){
      update_by_pointer(buffer_data8.data,   LARGE_UNALIGNED_BUF_LEN8, 1, 1, 'c');
      rv = check(buffer_data8.overflow,  1,  1,  'c');
    }
    break;
  case 5: // data underflow
    if(large_ind == 1){
      update_by_pointer(buffer_data1.data,  -1,  1,  1,  'c');
      rv = partial_check(buffer_data1.underflow,  LARGE_UNALIGNED_BUF_LEN1-1,  1,  1,  'c');
    }else if(large_ind == 2){
      update_by_pointer(buffer_data2.data,  -1,  1,  1,  'c');
      rv = partial_check(buffer_data2.underflow,  LARGE_UNALIGNED_BUF_LEN2-1,  1,  1,  'c');
    }else if(large_ind == 3){
      update_by_pointer(buffer_data3.data,  -1,  1,  1,  'c');
      rv = partial_check(buffer_data3.underflow,  LARGE_UNALIGNED_BUF_LEN3-1,  1,  1,  'c');
    }else if(large_ind == 4){
      update_by_pointer(buffer_data4.data,  -1,  1,  1,  'c');
      rv = partial_check(buffer_data4.underflow,  LARGE_UNALIGNED_BUF_LEN4-1,  1,  1,  'c');
    }else if(large_ind == 5){
      update_by_pointer(buffer_data5.data,  -1,  1,  1,  'c');
      rv = partial_check(buffer_data5.underflow,  LARGE_UNALIGNED_BUF_LEN5-1,  1,  1,  'c');
    }else if(large_ind == 6){
      update_by_pointer(buffer_data6.data,  -1,  1,  1,  'c');
      rv = partial_check(buffer_data6.underflow,  LARGE_UNALIGNED_BUF_LEN6-1,  1,  1,  'c');
    }else if(large_ind == 7){
      update_by_pointer(buffer_data7.data,  -1,  1,  1,  'c');
      rv = partial_check(buffer_data7.underflow,  LARGE_UNALIGNED_BUF_LEN7-1,  1,  1,  'c');
    }else if(large_ind == 8){
      update_by_pointer(buffer_data8.data,  -1,  1,  1,  'c');
      rv = partial_check(buffer_data8.underflow,  LARGE_UNALIGNED_BUF_LEN8-1,  1,  1,  'c');
    }
    break;
  default:
    rv = -1;
  }

  delete buffer_heap1;
  delete buffer_heap2;
  delete buffer_heap3;
  delete buffer_heap4;
  delete buffer_heap5;
  delete buffer_heap6;
  delete buffer_heap7;
  delete buffer_heap8;
  return rv;
}