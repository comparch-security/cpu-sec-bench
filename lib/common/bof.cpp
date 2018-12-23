#include "include/bof.hpp"

void char_buffer_init(charBuffer *cb) {
  for(unsigned int i=0; i<7; i++) {
    cb->underflow[i] = 'u';
    cb->data[i]      = 'd';
    cb->overflow[i]  = 'o';
  }
  cb->underflow[7] = 0;
  cb->data[7]      = 0;
  cb->overflow[7]  = 0;
}

void update_index(charBuffer& cb, int size, int step) {
  for(int i=0; i!= size; i += step)
    cb.data[i] = 'c';
}

void update_pointer(char *buf, int size, int step) {
  for(int i=0; i!= size; i += step, buf += step)
    *buf = 'c';
}

int check(const char *buf) {
  for(unsigned int i=0; i<7; i++)
    if(buf[i] != 'c') return 1;
  return 0;
}
