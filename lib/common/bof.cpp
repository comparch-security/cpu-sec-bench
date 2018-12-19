#include "include/bof.hpp"

void char_buffer_init(volatile charBuffer *cb) {
  for(unsigned int i=0; i<7; i++) {
    cb->underflow[i] = 'u';
    cb->data[i]      = 'd';
    cb->overflow[i]  = 'o';
  }
  cb->underflow[7] = 0;
  cb->data[7]      = 0;
  cb->overflow[7]  = 0;
}
