#ifndef BOF_HPP_INCLUDED
#define BOF_HPP_INCLUDED

struct charBuffer
{
  char underflow[8];
  char data[8];
  char overflow[8];
};

class smallObject
{
  public:
    char data[8] = "ddddddd";
  private:
    char overflow[8] = "fffffff";
};
class hugeObject
{
  public:
    char data[64];
};
struct objectBuffer
{
  class smallObject data;
  char overflow[8] = "ooooooo";
};

extern void char_buffer_init(charBuffer *, char uf, char d, char of);
extern void update_by_index(charBuffer& cb, long long offset, long long size, int step, char c);
extern void update_by_pointer(char *buf, long long offset, long long size, int step, char c);
extern int read_by_index(const charBuffer& cb, long long offset, long long size, int step, char c);
extern int read_by_pointer(const char *buf, long long offset, long long size, int step, char c);
extern int check(const char *buf, int size, int step, char c);

#endif
