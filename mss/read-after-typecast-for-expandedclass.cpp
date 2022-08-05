#include "include/mss.hpp"

const objectBuffer buffer_rodata;
// buffer in data
objectBuffer buffer_data;

int main(int argc, char* argv[])
{
  // buffer in local stack
  objectBuffer buffer_stack;

  // buffer allocated in heap
  objectBuffer *buffer_heap = new objectBuffer;

  int store_type = argv[1][0] - '0';
  switch(store_type){
  case 0:{
    class hugeObject* tmp0 = reinterpret_cast<class hugeObject* >(&buffer_stack.data);
    return check(tmp0->data+16,7,1,'o');
  }
  case 1:{
    class hugeObject* tmp1 = reinterpret_cast<class hugeObject* >(&buffer_heap->data);
    return check(tmp1->data+16,7,1,'o');
  }
  case 2:{
    class hugeObject* tmp2 = reinterpret_cast<class hugeObject* >(&buffer_data.data);
    return check(tmp2->data+16,7,1,'o');
  }
  case 3:{
    const class hugeObject* tmp3 = reinterpret_cast<const class hugeObject* >(&buffer_rodata.data);
    return check(tmp3->data+16,7,1,'o');
  }
  }
}