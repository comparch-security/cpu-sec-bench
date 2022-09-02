#ifndef BOF_HPP_INCLUDED
#define BOF_HPP_INCLUDED
#include <iostream>
#define CB_BUF_LEN 8

class charBuffer
{
public:
  char underflow[CB_BUF_LEN];
  char data[CB_BUF_LEN];
  char overflow[CB_BUF_LEN];
public:
  charBuffer() = default;
  charBuffer(const char uf, const char d, const char of);
  void updateBuffer(const char uf, const char d, const char of);
  template<typename CompElem>
  CompElem getelem_byindex (int i, long long offset)const
  {int ind = offset+i; return data[ind];}
  template<typename CompElem>
  CompElem getelem_byindex (int i, int bufferid)const
  {
    std::cerr << "in getelem, i is: " << i << std::endl;
    switch (bufferid)
    {
    case 0:
      return underflow[i];
    case 1:
      return data[i];
    case 2:
      return overflow[i];   
    default:
      return data[i];
    }
  }
};

template<typename BufferType, typename CompElem>
int read_by_index(BufferType&& bu,  long long offset, long long size, int step, CompElem ce){
  for(int i=0; i != size; i += step){
    if(bu.template getelem_byindex<CompElem>(i,offset) != ce) return 1;
  }
  return 0;
}

template<typename BufferType, typename CompElem>
int check(BufferType&& bu, int size, int step, CompElem ce, int ind){
  std::cerr << "in funciton" << std::endl;
  for(int i=0; i != size; i += step){
    std::cerr << "val is: " << bu.template getelem_byindex<CompElem>(i,ind) << std::endl;
    if(bu.template getelem_byindex<CompElem>(i,ind) != ce) return 1;
  }
  return 0;
}

extern void update_by_index(charBuffer& cb, long long offset, long long size, int step, char c);
extern void update_by_pointer(char *buf, long long offset, long long size, int step, char c);
extern int read_by_pointer(const char *buf, long long offset, long long size, int step, char c);
//extern int check(const char *buf, int size, int step, char c);

#endif
