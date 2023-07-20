#include "include/assembly.hpp"
#include <stdlib.h>

// #define DEBUG_OUTPUT

#ifdef DEBUG_OUTPUT
#include <iostream>
#endif

void get_got_func(void **gotp, void *label, int cet) {
  char *pc = (char *)label;
  arch_int_t offset = 0;
  int instr;

#ifdef DEBUG_OUTPUT
  std::cout << "label to rand(): " << std::hex << (unsigned long long)pc << " " << offset << std::endl;
#endif

  // get the plt addr from the call instruction
  // assembly:
  //   894:   f9dff0ef                jal     ra,830 <rand@plt>
  instr = *(int *)pc;
  // get the imm20
  offset += ((instr >> 31) & 0x1)   << 20;
  offset += ((instr >> 21) & 0x3ff) << 1;
  offset += ((instr >> 20) & 0x1)   << 11;
  offset += ((instr >> 12) & 0xff)  << 12;
  if(offset & 0x00100000) offset |= 0xfffffffffff00000llu; // sign extension
  pc += offset;

#ifdef DEBUG_OUTPUT
  std::cout << "pos of PLT stub: " << std::hex << (unsigned long long)pc << " " << offset << std::endl;
#endif

  // get the GOT location from plt
  // assembly:
  //   c60:   00002e17                auipc   t3,0x2
  //   c64:   428e3e03                ld      t3,1064(t3) # 3088 <rand@GLIBC_2.27>
  //   c68:   000e0367                jalr    t1,t3
  instr = *(int *)pc;
  offset = instr & 0xfffff000;
  if(offset & 0x80000000) offset |= 0xffffffff00000000llu; // sign extension
  instr = *(int *)(pc+4);
  pc += offset;
  offset = (instr & 0xfff00000) >> 20;
  if(offset & 0x800) offset |= 0xfffffffffffff000llu; // sign extension
  pc += offset;

  *gotp = *(void **)pc;

#ifdef DEBUG_OUTPUT
  std::cout << "pos of GOT entry: " << std::hex << (unsigned long long)pc << " " << (unsigned long long)gotp << " " << offset << std::endl;
  std::cout << "instruction at the entry of rand(): " << std::hex << **((int **)gotp) << std::endl; // this instruction should remain even with ASLR
#endif
}

void replace_got_func(void **fake, void *got) {
 
  asm volatile(
    "sd   %0, 0(%1);"
    : : "r"(fake), "r"(got)
  );
}

#undef GET_GOT_LOC
