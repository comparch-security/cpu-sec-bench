#include "include/mss.hpp"
#include <cstdio>
#include <cstring>
#include <iterator>
#include <iostream>

//#define NDEBUG

#ifndef NDEBUG
char* long2charArr(long long  num) {
	
	char* arr=new char[8];
	int wei=56;
	for(int i=7;i>=0;i--){
		
		long long temp=num<<wei;
    arr[i]=temp>>56;
    std::cerr << "i = " << i << arr[i] << std::endl;
		wei-=8;
	}
	return arr;
}
#endif

static int read_after_typecast(const char* cb, size_t offset, size_t len, size_t step, char c);
bool is_little_endian(void);

const charBuffer buffer_rodata = {"uuuuuuu","ddddddd","ooooooo"};
charBuffer buffer_data;
int ret;

int main(int argc, char* argv[])
{
  // buffer in data
  char_buffer_init(&buffer_data, 'u', 'd', 'o');

  // buffer in local stack
  charBuffer buffer_stack;
  char_buffer_init(&buffer_stack, 'u', 'd', 'o');

  // buffer allocated in heap
  charBuffer *buffer_heap = new charBuffer;
  char_buffer_init(buffer_heap, 'u', 'd', 'o');

  size_t char_len = sizeof(char);
  size_t ll_len = sizeof(long long);

  int store_type = argv[1][0] - '0';
  switch(store_type){
  case 0:{
      #ifndef NDEBUG
          std::cerr << "the pointer of underflow is: " << reinterpret_cast<void*>(buffer_stack.underflow) << std::endl;
          std::cerr << "the pointer of data before cast is: " << reinterpret_cast<void*>(buffer_stack.data) << std::endl;
          std::cerr << "the pointer of overflow is: " << reinterpret_cast<void*>(buffer_stack.overflow) << std::endl;
      #endif
      return read_after_typecast(std::end(buffer_stack.data)-char_len,char_len,ll_len-char_len,char_len,'u');
  }
  case 1:{
      #ifndef NDEBUG
          std::cerr << "the pointer of underflow is: " << reinterpret_cast<void*>(buffer_heap->underflow) << std::endl;
          std::cerr << "the pointer of data before cast is: " << reinterpret_cast<void*>(buffer_heap->data) << std::endl;
          std::cerr << "the pointer of overflow is: " << reinterpret_cast<void*>(buffer_heap->overflow) << std::endl;
      #endif
    return read_after_typecast(std::end(buffer_heap->data)-char_len,char_len,ll_len-char_len,char_len,'d');
  }
  case 2:{
    #ifndef NDEBUG
        std::cerr << "the pointer of underflow is: " << reinterpret_cast<void*>(buffer_data.underflow) << std::endl;
        std::cerr << "the pointer of data before cast is: " << reinterpret_cast<void*>(buffer_data.data) << std::endl;
        std::cerr << "the pointer of overflow is: " << reinterpret_cast<void*>(buffer_data.overflow) << std::endl;
    #endif
    return read_after_typecast(std::end(buffer_data.data)-char_len,char_len,ll_len-char_len,char_len,'d');
  }
  case 3:{
    #ifndef NDEBUG
        std::cerr << "the pointer of underflow is: " << reinterpret_cast<const void*>(buffer_rodata.underflow) << std::endl;
        std::cerr << "the pointer of data before cast is: " << reinterpret_cast<const void*>(buffer_rodata.data) << std::endl;
        std::cerr << "the pointer of overflow is: " << reinterpret_cast<const void*>(buffer_rodata.overflow) << std::endl;
    #endif
    return read_after_typecast(std::end(buffer_rodata.data)-char_len,char_len,ll_len-char_len,char_len,'d');
  }
  }

}

static int read_after_typecast(const char* cb, size_t offset, size_t len, size_t step, char c){
    char* b = const_cast<char*>(cb);
    long long* ptr = reinterpret_cast<long long*>(b);
    long long tmp = *ptr;

    // bool res = is_little_endian();

    // if(res){
    //     tmp = (*ptr) >> 8*sizeof(char);
    // }else{
    //     tmp = *ptr;
    // }

    int devi = 8*(sizeof(char)-1);

    #ifndef NDEBUG
      std::cerr << "baiscially cast long long is: " << tmp << std::endl;
      std::cerr << "long long -- pointer addr is: " << reinterpret_cast<void*>(ptr-1) << std::endl;
      std::cerr << "long long  pointer addr is: " << reinterpret_cast<void*>(ptr) << std::endl;
      std::cerr << "long long ++ pointer addr is: " << reinterpret_cast<void*>(ptr+1) << std::endl;
      std::cerr << "reverse baiscially cast char* is: " << long2charArr(tmp) << std::endl;
    #endif

    for(size_t i= 0; i < len; i+=step){
        
        char cmp = static_cast<char>(tmp & ((0xff << devi) & 0xffffffff));
        #ifndef NDEBUG
          std::cerr << cmp << std::endl;
        #endif
        if(cmp != c){
              return 1;
        }
        tmp >>= step*8;
    }
    return 0;
}

bool is_little_endian(void){

  int a = 1;
  int res = static_cast<int>(*reinterpret_cast<char*>(&a));
  if(res == 1) return true;
  else return false;

}

