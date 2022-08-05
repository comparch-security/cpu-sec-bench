#include "include/mss.hpp"

void char_buffer_init(charBuffer *cb, char uf, char d, char of) {
  for(unsigned int i=0; i<7; i++) {
    cb->underflow[i] = uf;
    cb->data[i]      = d;
    cb->overflow[i]  = of;
  }
  cb->underflow[7] = 0;
  cb->data[7]      = 0;
  cb->overflow[7]  = 0;
}

void update_by_index(charBuffer& cb, long long offset, long long size, int step, char c) {
  for(long long i=offset; i != size+offset; i += step)
    cb.data[i] = c;
}

void update_by_pointer(char *buf, long long offset, long long size, int step, char c) {
  buf += offset;
  for(long long i=0; i != size; i += step, buf += step)
    *buf = c;
}

int read_by_index(const charBuffer& cb, long long offset, long long size, int step, char c) {
  for(long long i=offset; i != size+offset; i += step)
    if(cb.data[i] != c) return 1;
  return 0;
}

void update_by_typecast(char *buf, long long val){
  long long *ptr = reinterpret_cast<long long*>(buf);
  *ptr = val;
}

int read_by_pointer(const char *buf, long long offset, long long size, int step, char c) {
  buf += offset;
  for(long long i=0; i != size; i += step, buf += step)
    if(*buf != c) return 1;
  return 0;
}

int read_by_typecast(const char* cb, int offset, int len, int step, char c){
    char* b = const_cast<char*>(cb);
    long long* ptr = reinterpret_cast<long long*>(b);
    /* check every char element in array except '\0' */
    long long tmp = (*ptr) >> 8;

    for(int i= 0; i != len; i+=step){
      char cmp = static_cast<char>(tmp & 0xff);
      if(cmp != c) return 1;
      tmp >>= step*8;
    }
    return 0;
}

int check(const char *buf, int size, int step, char c) {
  for(int i=0; i!= size; i += step)
    if(buf[i] != c) return 1;
  return 0;
}
