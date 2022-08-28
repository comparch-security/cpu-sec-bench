#ifndef BOF_HPP_INCLUDED
#define BOF_HPP_INCLUDED

#define BUFFERLEN 8
#define LASTELEM  7

class charBuffer
{
public:
  char underflow[BUFFERLEN];
  char data[BUFFERLEN];
  char overflow[BUFFERLEN];
public:
  charBuffer() = default;
  charBuffer(const char uf, const char d, const char of);
  void updateBuffer(const char uf, const char d, const char of);
};

extern void update_by_index(charBuffer& cb, long long offset, long long size, int step, char c);
extern void update_by_pointer(char *buf, long long offset, long long size, int step, char c);
extern int read_by_index(const charBuffer& cb, long long offset, long long size, int step, char c);
extern int read_by_pointer(const char *buf, long long offset, long long size, int step, char c);
extern int check(const char *buf, int size, int step, char c);

#endif
