#ifndef BOF_HPP_INCLUDED
#define BOF_HPP_INCLUDED

struct charBuffer
{
  char underflow[8];
  char data[8];
  char overflow[8];
};

extern void char_buffer_init(charBuffer *, char uf, char d, char of);
extern void update_index(charBuffer& cb, int size, int step, char c);
extern void update_pointer(char *buf, int size, int step, char c);
extern int read_index(const charBuffer& cb, int offset, int size, int step, char c);
extern int read_pointer(const char *buf, int offset, int size, int step, char c);
extern int check(const char *buf, int size, int step, char c);

#endif
