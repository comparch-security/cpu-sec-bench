#include "include/mss.hpp"
//This test is used to find vlunerblity of compressed capability

// buffer in rodata
const LargeMemberBuffer<0x100>  buffer_rodata1('u','d','o');
const LargeMemberBuffer<0x200>  buffer_rodata2('u','d','o');
const LargeMemberBuffer<0x400>  buffer_rodata3('u','d','o');
const LargeMemberBuffer<0x800>  buffer_rodata4('u','d','o');
const LargeMemberBuffer<0x1000> buffer_rodata5('u','d','o');
const LargeMemberBuffer<0x2000> buffer_rodata6('u','d','o');
const LargeMemberBuffer<0x4000> buffer_rodata7('u','d','o');
const LargeMemberBuffer<0x8000> buffer_rodata8('u','d','o');
const LargeMemberBufferBase * buffer_rodata[] = {
  &buffer_rodata1， &buffer_rodata2, &buffer_rodata3， &buffer_rodata4,
  &buffer_rodata5， &buffer_rodata6, &buffer_rodata7， &buffer_rodata8
};

// buffer in data
LargeMemberBuffer<0x100>  buffer_data1('u','d','o');
LargeMemberBuffer<0x200>  buffer_data2('u','d','o');
LargeMemberBuffer<0x400>  buffer_data3('u','d','o');
LargeMemberBuffer<0x800>  buffer_data4('u','d','o');
LargeMemberBuffer<0x1000> buffer_data5('u','d','o');
LargeMemberBuffer<0x2000> buffer_data6('u','d','o');
LargeMemberBuffer<0x4000> buffer_data7('u','d','o');
LargeMemberBuffer<0x8000> buffer_data8('u','d','o');
LargeMemberBufferBase * buffer_data[] = {
  &buffer_data1， &buffer_data2, &buffer_data3， &buffer_data4,
  &buffer_data5， &buffer_data6, &buffer_data7， &buffer_data8
};

const int buf_lens[] = {0x100, 0x200, 0x400, 0x800, 0x1000, 0x2000, 0x4000, 0x8000};

int main(int argc, char* argv[])
{
  // buffer in local stack
  LargeMemberBuffer<0x100>  buffer_stack1('u','d','o');
  LargeMemberBuffer<0x200>  buffer_stack2('u','d','o');
  LargeMemberBuffer<0x400>  buffer_stack3('u','d','o');
  LargeMemberBuffer<0x800>  buffer_stack4('u','d','o');
  LargeMemberBuffer<0x1000> buffer_stack5('u','d','o');
  LargeMemberBuffer<0x2000> buffer_stack6('u','d','o');
  LargeMemberBuffer<0x4000> buffer_stack7('u','d','o');
  LargeMemberBuffer<0x8000> buffer_stack8('u','d','o');
  LargeMemberBufferBase * buffer_stack[] = {
    &buffer_stack1， &buffer_stack2, &buffer_stack3， &buffer_stack4,
    &buffer_stack5， &buffer_stack6, &buffer_stack7， &buffer_stack8
  };

  // buffer allocated in heap
  LargeMemberBufferBase * buffer_heap[] = {
    new LargeMemberBuffer<0x100>('u','d','o'),
    new LargeMemberBuffer<0x200>('u','d','o'),
    new LargeMemberBuffer<0x400>('u','d','o'),
    new LargeMemberBuffer<0x800>('u','d','o'),
    new LargeMemberBuffer<0x1000>('u','d','o'),
    new LargeMemberBuffer<0x2000>('u','d','o'),
    new LargeMemberBuffer<0x4000>('u','d','o'),
    new LargeMemberBuffer<0x8000>('u','d','o')
  };

  int store_type = argv[1][0] - '0';
  int flow_type  = argv[2][0] - '0';
  int large_ind  = argv[3][0] - '0';
  large_ind++; // match with vector index

  int rv;
  
  switch(store_type*2+flow_type) {
  case 0: // stack overflow
    rv =  read_by_pointer(buffer_stack[large_ind]->get_data(), buf_lens[large_ind], 1, 1, 'o');
    break;
  case 1: // stack underflow
    rv = read_by_pointer(buffer_stack[large_ind]->get_data(),   -2, 1, 1, 'u');
    break;
  case 2: // heap overflow
    rv = read_by_pointer(buffer_heap[large_ind]->get_data(),   buf_lens[large_ind], 1, 1, 'o');
    break;
  case 3: // heap underflow
    rv = read_by_pointer(buffer_heap[large_ind]->get_data(),   -2, 1, 1, 'u');
    break;
  case 4: // data overflow
    rv = read_by_pointer(buffer_data[large_ind]->get_data(),   buf_lens[large_ind], 1, 1, 'o');
    break;
  case 5: // data underflow
    rv = read_by_pointer(buffer_data[large_ind]->get_data(),   -2, 1, 1, 'u');
    break;
  case 6: // rodata overflow
    rv = read_by_pointer(buffer_rodata[large_ind]->get_data(),   buf_lens[large_ind], 1, 1, 'o');
    break;
  case 7: // rodata underflow
    rv = read_by_pointer(buffer_rodata[large_ind]->get_data(),   -2, 1, 1, 'u');
    break;
  default:
    rv = -1;
  }

  for(auto b:buffer_heap) delete b;
  return rv;
}
