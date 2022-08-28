#include "include/mss.hpp"

charBuffer::charBuffer(const char uf, const char d, const char of){
  for(unsigned int i=0; i!=LASTELEM; i++) {
    underflow[i] = uf;
    data[i] = d;
    overflow[i] = of;
  }
  underflow[LASTELEM] = 0;
  data[LASTELEM]      = 0;
  overflow[LASTELEM]  = 0;
}

void charBuffer::updateBuffer(const char uf, const char d, const char of){
  for(unsigned int i=0; i!=LASTELEM; i++) {
    underflow[i] = uf;
    data[i] = d;
    overflow[i] = of;
  }
  underflow[LASTELEM] = 0;
  data[LASTELEM]      = 0;
  overflow[LASTELEM]  = 0;
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

int read_by_pointer(const char *buf, long long offset, long long size, int step, char c) {
  buf += offset;
  for(long long i=0; i != size; i += step, buf += step)
    if(*buf != c) return 1;
  return 0;
}

int check(const char *buf, int size, int step, char c) {
  for(int i=0; i!= size; i += step)
    if(buf[i] != c) return 1;
  return 0;
}
