#include "include/mss.hpp"

// buffer in data
LargeMemberBuffer<0x101>  buffer_data1('u','d','o');
LargeMemberBuffer<0x201>  buffer_data2('u','d','o');
LargeMemberBuffer<0x401>  buffer_data3('u','d','o');
LargeMemberBuffer<0x801>  buffer_data4('u','d','o');
LargeMemberBuffer<0x1001> buffer_data5('u','d','o');
LargeMemberBuffer<0x2001> buffer_data6('u','d','o');
LargeMemberBuffer<0x4001> buffer_data7('u','d','o');
LargeMemberBuffer<0x8001> buffer_data8('u','d','o');
LargeMemberBuffer<0xffff> buffer_data9('u','d','o');
LargeMemberBufferBase * buffer_data[] = {
  &buffer_data1, &buffer_data2, &buffer_data3, &buffer_data4,
  &buffer_data5, &buffer_data6, &buffer_data7, &buffer_data8, &buffer_data9
};

const int buf_lens[] = {0x101, 0x201, 0x401, 0x801, 0x1001, 0x2001, 0x4001, 0x8001, 0xffff};

int main(int argc, char* argv[])
{
  // buffer in local stack
  LargeMemberBuffer<0x101>  buffer_stack1('u','d','o');
  LargeMemberBuffer<0x201>  buffer_stack2('u','d','o');
  LargeMemberBuffer<0x401>  buffer_stack3('u','d','o');
  LargeMemberBuffer<0x801>  buffer_stack4('u','d','o');
  LargeMemberBuffer<0x1001> buffer_stack5('u','d','o');
  LargeMemberBuffer<0x2001> buffer_stack6('u','d','o');
  LargeMemberBuffer<0x4001> buffer_stack7('u','d','o');
  LargeMemberBuffer<0x8001> buffer_stack8('u','d','o');
  LargeMemberBuffer<0xffff> buffer_stack9('u','d','o');
  LargeMemberBufferBase * buffer_stack[] = {
    &buffer_stack1, &buffer_stack2, &buffer_stack3, &buffer_stack4,
    &buffer_stack5, &buffer_stack6, &buffer_stack7, &buffer_stack8, &buffer_stack9
  };

  // buffer allocated in heap
  LargeMemberBufferBase * buffer_heap[] = {
    new LargeMemberBuffer<0x101>('u','d','o'),
    new LargeMemberBuffer<0x201>('u','d','o'),
    new LargeMemberBuffer<0x401>('u','d','o'),
    new LargeMemberBuffer<0x801>('u','d','o'),
    new LargeMemberBuffer<0x1001>('u','d','o'),
    new LargeMemberBuffer<0x2001>('u','d','o'),
    new LargeMemberBuffer<0x4001>('u','d','o'),
    new LargeMemberBuffer<0x8001>('u','d','o'),
    new LargeMemberBuffer<0xffff>('u','d','o')
  };

  int store_type = argv[1][0] - '0';
  int flow_type  = argv[2][0] - '0';
  int large_ind  = argv[3][0] - '0';

  int rv;

  switch(store_type*2+flow_type) {
  case 0: // stack overflow
    update_by_pointer(buffer_stack[large_ind]->get_data(), buf_lens[large_ind], 1, 1, 'c');
    rv = check(buffer_stack[large_ind]->get_overflow(), 1, 1, 'c');
    break;
  case 1: // stack underflow
    update_by_pointer(buffer_stack[large_ind]->get_data(), -1, 1, 1, 'c');
    rv = partial_check(buffer_stack[large_ind]->get_underflow(), buf_lens[large_ind]-1, 1, 1, 'c');
    break;
  case 2: // heap overflow
    update_by_pointer(buffer_heap[large_ind]->get_data(), buf_lens[large_ind], 1, 1, 'c');
    rv = check(buffer_heap[large_ind]->get_overflow(), 1, 1, 'c');
    break;
  case 3: // heap underflow
    update_by_pointer(buffer_heap[large_ind]->get_data(), -1, 1, 1, 'c');
    rv = partial_check(buffer_heap[large_ind]->get_underflow(), buf_lens[large_ind]-1, 1, 1, 'c');
    break;
  case 4: // data overflow
    update_by_pointer(buffer_data[large_ind]->get_data(), buf_lens[large_ind], 1, 1, 'c');
    rv = check(buffer_data[large_ind]->get_overflow(), 1, 1, 'c');
    break;
  case 5: // data underflow
    update_by_pointer(buffer_heap[large_ind]->get_data(), -1, 1, 1, 'c');
    rv = partial_check(buffer_heap[large_ind]->get_underflow(), buf_lens[large_ind]-1, 1, 1, 'c');
    break;
  default:
    rv = -1;
  }

  for(auto b:buffer_heap) delete b;
  return rv;
}