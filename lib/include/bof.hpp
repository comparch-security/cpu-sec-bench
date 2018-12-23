#ifndef BOF_HPP_INCLUDED
#define BOF_HPP_INCLUDED

struct charBuffer
{
  char underflow[8];
  char data[8];
  char overflow[8];
};

extern void char_buffer_init(charBuffer *);
extern void update_index(charBuffer& cb, int size, int step);
extern void update_pointer(char *buf, int size, int step);
extern int check(const char *buf);

#endif
