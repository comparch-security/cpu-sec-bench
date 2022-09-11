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

template<typename tT>
class typeCastBuffer
{
public:
  tT target;
  char data[CB_BUF_LEN];

  typeCastBuffer() = default;
  typeCastBuffer(tT t, char d)
    : target(t)
  {
    for(unsigned int i=0; i!=CB_BUF_LEN-1; i++) data[i] = d;
    data[CB_BUF_LEN-1] = 0;
  }
};

extern void update_by_index(charBuffer& cb, long long offset, long long size, int step, char c);
extern void update_by_pointer(char *buf, long long offset, long long size, int step, char c);
extern int read_by_index(const charBuffer& cb, long long offset, long long size, int step, char c);

template<typename T>
int read_by_pointer(const T *buf, long long offset, long long size, int step, T c) {
  buf += offset;
  for(long long i=0; i != size; i += step, buf += step)
    if(*buf != c) return 1;
  return 0;
}

template<typename T>
int check(const T *buf, int size, int step, T c) {
  for(int i=0; i!= size; i += step)
    if(buf[i] != c) return 1;
  return 0;
}

#endif
