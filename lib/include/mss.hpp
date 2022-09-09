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

class typeCastBuffer
{
public:
  long long target;
  char data[CB_BUF_LEN];

  typeCastBuffer() = default;
  typeCastBuffer(long long t, char d);
};

class scalarCastBuffer
{
public:
  int target;
  int data;
  scalarCastBuffer() = default;
  scalarCastBuffer(int d):target(d),data(d){};
};

extern void update_by_index(charBuffer& cb, long long offset, long long size, int step, char c);
extern void update_by_pointer(char *buf, long long offset, long long size, int step, char c);
extern int read_by_index(const charBuffer& cb, long long offset, long long size, int step, char c);
extern int read_by_pointer(const char *buf, long long offset, long long size, int step, char c);

template<typename T>
int check(const T *buf, int size, int step, T c) {
  for(int i=0; i!= size; i += step)
    if(buf[i] != c) return 1;
  return 0;
}

#endif
