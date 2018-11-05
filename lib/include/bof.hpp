#ifndef BOF_HPP_INCLUDED
#define BOF_HPP_INCLUDED

//reliable ways to get the adresses of the return address and old base pointer
// interesting, investigating __builtin_return_address, -fno-optimize-sibling-calls -fno-omit-frame-pointer, wsong83
#define OLD_BP_PTR   __builtin_frame_address(0)
#define RET_ADDR_PTR ((void**)OLD_BP_PTR + 1)

struct charBuffer
{
  char underflow[8];
  char data[8];
  char overflow[8];

  charBuffer() {
    for(unsigned int i=0; i<7; i++) {
      underflow[i] = 'u';
      data[i]      = 'd';
      overflow[i]  = 'o';
    }
    underflow[7] = 0;
    data[7]      = 0;
    overflow[7]  = 0;
  }
};

#endif

