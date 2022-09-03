#ifndef BOF_HPP_INCLUDED
#define BOF_HPP_INCLUDED

#define CB_BUF_LEN 8

class charBuffer
{
public:
  char underflow[CB_BUF_LEN];
  char data[CB_BUF_LEN];
  char overflow[CB_BUF_LEN];

  charBuffer() = default;
  charBuffer(const char uf, const char d, const char of);
  void updateBuffer(const char uf, const char d, const char of);
};

class crossptrBuffer
{
public:
  long long target;
  char data[CB_BUF_LEN];

  crossptrBuffer() = default;
  crossptrBuffer(long long t, char d);
};

extern void update_by_index(charBuffer& cb, long long offset, long long size, int step, char c);
extern void update_by_pointer(char *buf, long long offset, long long size, int step, char c);
extern int read_by_index(const charBuffer& cb, long long offset, long long size, int step, char c);
extern int read_by_pointer(const char *buf, long long offset, long long size, int step, char c);
extern int check(const char *buf, int size, int step, char c);

#endif
