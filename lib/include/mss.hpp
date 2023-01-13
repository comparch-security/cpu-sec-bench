#ifndef BOF_HPP_INCLUDED
#define BOF_HPP_INCLUDED

#include "include/gcc_builtin.hpp"
#define CB_BUF_LEN 8
#define LARGE_UNALIGNED_BUF_LEN 0xfff0

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

class LargeMemberBuffer
{
public:
  char underflow[LARGE_UNALIGNED_BUF_LEN];
  char data[LARGE_UNALIGNED_BUF_LEN];
  char overflow[LARGE_UNALIGNED_BUF_LEN];

  LargeMemberBuffer() = default;
  LargeMemberBuffer(const char uf, const char d, const char of);
  void updateBuffer(const char uf, const char d, const char of);
};

template<typename tT, unsigned int CB_SIZE = CB_BUF_LEN>
class typeCastBuffer
{
public:
  tT target;
  char data[CB_SIZE];

  typeCastBuffer() = default;
  typeCastBuffer(tT t, char d)
    : target(t)
  {
    for(unsigned int i=0; i!=CB_SIZE-1; i++) data[i] = d;
    data[CB_SIZE-1] = 0;
  }
};

extern void update_by_index(charBuffer& cb, long long offset, long long size, int step, char c);
extern void FORCE_NOINLINE update_by_pointer(char *buf, long long offset, long long size, int step, char c);
extern int read_by_index(const charBuffer& cb, long long offset, long long size, int step, char c);

template<typename T>
int read_by_pointer(const T *buf, long long offset, long long size, int step, T c) {
  buf += offset;
  for(long long i=0; i != size; i += step, buf += step)
    if(*buf != c) return 1;
  return 0;
}

template<typename T> FORCE_NOINLINE
int check(const T *buf, int size, int step, T c) {
  for(int i=0; i!= size; i += step)
    if(buf[i] != c) return 1;
  return 0;
}

#endif
