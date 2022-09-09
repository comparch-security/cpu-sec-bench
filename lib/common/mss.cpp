#include "include/mss.hpp"

charBuffer::charBuffer(const char uf, const char d, const char of){
  for(unsigned int i=0; i!=CB_BUF_LEN-1; i++) {
    underflow[i] = uf;
    data[i] = d;
    overflow[i] = of;
  }
  underflow[CB_BUF_LEN-1] = 0;
  data[CB_BUF_LEN-1]      = 0;
  overflow[CB_BUF_LEN-1]  = 0;
}

void charBuffer::updateBuffer(const char uf, const char d, const char of){
  for(unsigned int i=0; i!=CB_BUF_LEN-1; i++) {
    underflow[i] = uf;
    data[i] = d;
    overflow[i] = of;
  }
  underflow[CB_BUF_LEN-1] = 0;
  data[CB_BUF_LEN-1]      = 0;
  overflow[CB_BUF_LEN-1]  = 0;
}

typeCastBuffer::typeCastBuffer(long long t, char d)
  : target(t)
{
  for(unsigned int i=0; i!=CB_BUF_LEN-1; i++)
    data[i] = d;
  data[CB_BUF_LEN-1]      = 0;
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