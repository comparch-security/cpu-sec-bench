#ifndef BOF_HPP_INCLUDED
#define BOF_HPP_INCLUDED

struct charBuffer
{
  char underflow[8];
  char data[8];
  char overflow[8];
};

struct crossptrBuffer
{
  long long target;
  char data[8]; 
};

extern void char_buffer_init(charBuffer *, char uf, char d, char of);
extern void update_by_index(charBuffer& cb, long long offset, long long size, int step, char c);
extern void update_by_pointer(char *buf, long long offset, long long size, int step, char c);
extern int read_by_index(const charBuffer& cb, long long offset, long long size, int step, char c);
extern int read_by_pointer(const char *buf, long long offset, long long size, int step, char c);
extern int check(const char *buf, int size, int step, char c);

#endif
