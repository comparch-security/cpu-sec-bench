#include "include/assembly.hpp"
#include <stdlib.h>

//#define DEBUG_READ_GOT

#ifdef DEBUG_READ_GOT
#include <iostream>
#endif

int dummy_leaf_rv = 0;

int FORCE_NOINLINE dummy_leaf_func(int v) {
  dummy_leaf_rv += rand();
  return dummy_leaf_rv;
}

void get_got_func(void **gotp, void *label, int cet) {
  char *pc = (char *)label;
  if(cet) pc += 4; //bypass endbr64
  int offset = *(int *)(pc+1);
  pc += 5 + offset; // point to 1st instruction in plt

  if(cet) {
    pc += 4; // bypass endbr64
    offset = *(int *)(pc+3);
    pc += 7 + offset;
  } else {
    offset = *(int *)(pc+2);
    pc += 6 + offset;
  }

  *gotp = pc;

#ifdef DEBUG_READ_GOT
  std::cout << "instruction at the entry of rand(): " << std::hex << **((int **)pc) << std::endl; // this instruction should remain even with ASLR  
#endif
}

void replace_got_func(void **fake, void *got) {
  asm volatile(
    "movq %1, (%0);" // replace the GPT entry
    : : "r"(got), "r"(fake)
  );
}


