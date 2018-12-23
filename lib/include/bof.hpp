#ifndef BOF_HPP_INCLUDED
#define BOF_HPP_INCLUDED

struct charBuffer
{
  char underflow[8];
  char data[8];
  char overflow[8];
};

extern void char_buffer_init(volatile charBuffer *);

class buf
{
  public:
    char d[8];
};

struct buffer_struct
{
  buf underflow;
  buf data;
  buf overflow;

  buffer_struct()
  {
    for(unsigned int i=0; i<7; i++)
    {    
      underflow.d[i] = 'u';
      data.d[i] = 'd';
      overflow.d[i] = 'o';
    } 
    underflow.d[7] = 0;
    data.d[7] = 0;
    overflow.d[7] = 0;
  }
};

#endif
