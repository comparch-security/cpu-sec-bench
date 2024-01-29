#ifndef BOF_HPP_INCLUDED
#define BOF_HPP_INCLUDED
#include <iostream>
#include "include/builtin.hpp"
#define RELOC_NUM 1024

extern void FORCE_NOINLINE update_by_pointer(char *buf, long long offset, long long size, int step, char c);

template<typename T> FORCE_NOINLINE
int read_by_pointer(const T *buf, long long offset, long long size, int step, T c) {
  buf += offset;
  for(long long i=0; i != size; i += step, buf += step){
    if(fake_use_arg) std::cout << "buf[i] is: " << buf[i] << "buf + i is: " << (long long)&(buf[i]) << std::endl;
    if(*buf != c) return 1;
  }
  return 0;
}

template<typename T, unsigned len> FORCE_NOINLINE
int read_by_index(const T (&buf)[len], long long offset, long long size, int step, T c) {
for(long long i=offset; i != size+offset; i += step){
    if(fake_use_arg) std::cout << "buf[i] is: " << buf[i] << "buf + i is: " << (long long)&(buf[i]) << std::endl;
    if(buf[i] != c) return 1;
}
return 0;
}

template<typename T> FORCE_NOINLINE
int check(const T *buf, int size, int step, T c) {
  for(int i=0; i!= size; i += step){
    if(fake_use_arg) std::cout << "buf[i] is: " << buf[i] << "buf + i is: " << (long long)&(buf[i]) << std::endl;
    if(buf[i] != c) return 1;
  }
  return 0;
}

template<typename T> FORCE_INLINE
int inline_check(const T *buf, int size, int step, T c) {
  for(int i=0; i!= size; i += step){
    if(fake_use_arg) std::cout << "buf[i] is: " << buf[i] << "buf + i is: " << (long long)&(buf[i]) << std::endl;
    if(buf[i] != c) return 1;
  }
  return 0;
}

template<typename T> FORCE_NOINLINE
int partial_check(const T *buf, int offset, int size, int step, T c) {
  for(int i=offset; i!= offset+size; i += step){
    if(fake_use_arg) std::cout << "buf[i] is: " << buf[i] << "buf + i is: " << (long long)&(buf[i]) << std::endl;
    if(buf[i] != c) return 1;
  }
  return 0;
}

template<typename T, unsigned len> FORCE_NOINLINE
void update_by_index(T (&buf)[len], long long offset, long long size, int step, T c) {
  for(long long i=offset; i != size+offset; i += step){
    if(fake_use_arg) std::cout << "buf[i] is: " << buf[i] << "buf + i is: " << (long long)&(buf[i]) << std::endl;
      buf[i] = c;
  }
}

#endif
