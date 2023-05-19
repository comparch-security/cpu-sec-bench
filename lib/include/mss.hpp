#ifndef BOF_HPP_INCLUDED
#define BOF_HPP_INCLUDED

#include "include/gcc_builtin.hpp"
#define CB_BUF_LEN 8

#define LARGE_UNALIGNED_BUF_LEN1 0x100
#define LARGE_UNALIGNED_BUF_LEN2 0x200
#define LARGE_UNALIGNED_BUF_LEN3 0x400
#define LARGE_UNALIGNED_BUF_LEN4 0x800
#define LARGE_UNALIGNED_BUF_LEN5 0x1000
#define LARGE_UNALIGNED_BUF_LEN6 0x2000
#define LARGE_UNALIGNED_BUF_LEN7 0x4000
#define LARGE_UNALIGNED_BUF_LEN8 0x8000

#define LARGEMEMBERBUFFERCLASS(num)                                         \
                                                                            \
class LargeMemberBuffer##num                                                \
{                                                                           \
public:                                                                     \
  char underflow[LARGE_UNALIGNED_BUF_LEN##num];                             \
  char data[LARGE_UNALIGNED_BUF_LEN##num];                                  \
  char overflow[LARGE_UNALIGNED_BUF_LEN##num];                              \
                                                                            \
  LargeMemberBuffer##num() = default;                                       \
  LargeMemberBuffer##num(const char uf, const char d, const char of);       \
  void updateBuffer(const char uf, const char d, const char of);            \
}                                                                           \

#define LARGEMEMBERBUFFERCLASS_INIT(num)                                                      \
LargeMemberBuffer##num::LargeMemberBuffer##num(const char uf, const char d, const char of){   \
  for(unsigned int i=0; i!=LARGE_UNALIGNED_BUF_LEN##num-1; i++) {                             \
    underflow[i] = uf;                                                                        \
    data[i] = d;                                                                              \
    overflow[i] = of;                                                                         \
  }                                                                                           \
  underflow[LARGE_UNALIGNED_BUF_LEN##num-1] = 0;                                              \
  data[LARGE_UNALIGNED_BUF_LEN##num-1]      = 0;                                              \
  overflow[LARGE_UNALIGNED_BUF_LEN##num-1]  = 0;                                              \
}                                                                                             \

#define LARGEMEMBERBUFFERCLASS_UPDATE(num)                                                    \
void LargeMemberBuffer##num::updateBuffer(const char uf, const char d, const char of){        \
  for(unsigned int i=0; i!=LARGE_UNALIGNED_BUF_LEN##num-1; i++) {                             \
    underflow[i] = uf;                                                                        \
    data[i] = d;                                                                              \
    overflow[i] = of;                                                                         \
  }                                                                                           \
  underflow[LARGE_UNALIGNED_BUF_LEN##num-1] = 0;                                              \
  data[LARGE_UNALIGNED_BUF_LEN##num-1]      = 0;                                              \
  overflow[LARGE_UNALIGNED_BUF_LEN##num-1]  = 0;                                              \
}                                                                                             \

LARGEMEMBERBUFFERCLASS(1);
LARGEMEMBERBUFFERCLASS(2);
LARGEMEMBERBUFFERCLASS(3);
LARGEMEMBERBUFFERCLASS(4);
LARGEMEMBERBUFFERCLASS(5);
LARGEMEMBERBUFFERCLASS(6);
LARGEMEMBERBUFFERCLASS(7);
LARGEMEMBERBUFFERCLASS(8);

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
