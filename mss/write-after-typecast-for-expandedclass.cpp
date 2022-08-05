#include "include/mss.hpp"

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
    update_by_pointer(tmp0->data,16,7,1,'c');
    return check(tmp0->data+16,7,1,'c');
  }
  case 1:{
    class hugeObject* tmp1 = reinterpret_cast<class hugeObject* >(&buffer_heap->data);
    update_by_pointer(tmp1->data,16,7,1,'c');
    return check(tmp1->data+16,7,1,'c');
  }
  case 2:{
    class hugeObject* tmp2 = reinterpret_cast<class hugeObject* >(&buffer_data.data);
    update_by_pointer(tmp2->data,16,7,1,'c');
    return check(tmp2->data+16,7,1,'c');
  }
  }
}