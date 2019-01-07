#include "include/ptt.hpp"

void char_buffer_init(volatile charBuffer *cb) {
  for(unsigned int i=0; i<15; i++) 
    cb->data[i] = 'd';
  cb->data[15] = 0;
}
