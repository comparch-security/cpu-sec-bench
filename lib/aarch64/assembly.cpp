#include "include/assembly.hpp"
#include <stdlib.h>

//#define DEBUG_READ_GOT

#ifdef DEBUG_READ_GOT
#include <iostream>
#endif

int dummy_leaf_rv = 0;

int FORCE_NOINLINE dummy_leaf_func(int v) {
  return v + rand();
}

void get_got_func(void **gotp, void *label, int cet) {
  char *pc = (char *)label;
  arch_int_t offset = 0;

#ifdef DEBUG_READ_GOT
  std::cout << "label to rand(): " << std::hex << (unsigned long long)pc << " " << offset << std::endl;
#endif

  // on Clang 12.0.5 (MAC M1)
  // assembly:
  //   a1 01 00 94  bl      0x100003d50 <dyld_stub_binder+0x100003d50>
  //
  // bl: 100101, imm26
  // pc = pc + imm26 * 4
  offset = ((*(int *)pc) & 0x03ffffff); // get the imm26
  if(offset & 0x3000000) offset |= 0xfffffffffc000000llu; // sign extended to 64
  offset *= 4;
  pc += offset;

#ifdef DEBUG_READ_GOT
  std::cout << "pos of dyld stub: " << std::hex << (unsigned long long)pc << " " << offset << std::endl;
#endif

  // assembly
  //  1f 20 03 d5  nop
  //  f0 16 02 58  ldr     x16, 0x100008030 <dyld_stub_binder+0x100008030>
  //  00 02 1f d6  br      x16
  //
  //  ldr: 01011000, imm19, rd5
  //  pc = pc + imm19 * 4
  pc += 4;
  offset = ((*(int *)pc) & 0x00ffffe0) >> 5; // get the imm19
  if(offset & 0x40000) offset ^= 0xfffffffffff80000llu; // sign extended to 64
  offset *= 4;
  pc += offset;

#ifdef DEBUG_READ_GOT
  std::cout << "pos of GOT entry: " << std::hex << (unsigned long long)pc << " " << offset << std::endl;
#endif

  *gotp = pc;

#ifdef DEBUG_READ_GOT
  std::cout << "instruction at the entry of rand(): " << std::hex << **((int **)pc) << std::endl; // this instruction should remain even with ASLR
#endif
}

void replace_got_func(void **fake, void *got) {
  asm volatile(
    "str  %0, [%1];"   // replace the GOT entry
    : : "r"(fake), "r"(got)
  );

}

