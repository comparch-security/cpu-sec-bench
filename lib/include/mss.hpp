#ifndef BOF_HPP_INCLUDED
#define BOF_HPP_INCLUDED

#include "include/gcc_builtin.hpp"
#define CB_BUF_LEN 8

class LargeMemberBufferBase
{
  public:
  virtual void updateBuffer(const char uf, const char d, const char of) = 0;
  virtual const char * get_const_underflow() const = 0;
  virtual const char * get_const_data() const = 0;
  virtual const char * get_const_overflow() const = 0;
  virtual char * get_underflow() = 0;
  virtual char * get_data() = 0;
  virtual char * get_overflow() = 0;
  virtual ~LargeMemberBufferBase() = default;
};

template<unsigned int SZ>
class LargeMemberBuffer : public LargeMemberBufferBase
{
public:
  char underflow[SZ];
  char data[SZ];
  char overflow[SZ];

  LargeMemberBuffer() = default;
  LargeMemberBuffer(const char uf, const char d, const char of) {
    for(unsigned int i=0; i!=SZ-1; i++) {
      underflow[i] = uf;
      data[i] = d;
      overflow[i] = of;
    }
    underflow[SZ-1] = 0;
    data[SZ-1]      = 0;
    overflow[SZ-1]  = 0;
  }

  virtual void updateBuffer(const char uf, const char d, const char of) {
    for(unsigned int i=0; i!=SZ-1; i++) {
      underflow[i] = uf;
      data[i] = d;
      overflow[i] = of;
    }
    underflow[SZ-1] = 0;
    data[SZ-1]      = 0;
    overflow[SZ-1]  = 0;
  }

  virtual const char * get_const_underflow() const {
    return underflow;
  }

  virtual const char * get_const_data() const {
    return data;
  }

  virtual const char * get_const_overflow() const {
    return overflow;
  }

  virtual char * get_underflow() {
    return underflow;
  }

  virtual char * get_data() {
    return data;
  }

  virtual char * get_overflow() {
    return overflow;
  }

};

#define RELOC_NUM 64

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

template<typename T> FORCE_NOINLINE
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

template<typename T> FORCE_NOINLINE
int partial_check(const T *buf, int offset, int size, int step, T c) {
  for(int i=offset; i!= offset+size; i += step)
    if(buf[i] != c) return 1;
  return 0;
}

#endif
