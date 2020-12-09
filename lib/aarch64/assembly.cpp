#include "include/assembly.hpp"
#include <stdlib.h>

int dummy_leaf_rv = 0;

int FORCE_NOINLINE dummy_leaf_func(int v) {
  return v + rand();
}

void replace_got_func(void **org, void **fake) {
  char *pc = NULL, *pcc = NULL;
  unsigned int inst = 0;
  unsigned long long offset;

  // we know rand is call immediately afterwards
  // so get the plt address from ra
  asm volatile(
    "mov  %0, x30;"
    "ldr  w0, [x30];"
    "mov  %1,x0;"
    : "+r"(pc), "+r"(inst)
  );

  // get the 26-imm offset from the BL instruction
  offset = (inst & 0x03ffffff) << 2;

  // handle negative offset
  if(offset & 0x08000000) {
    offset |= 0xfffffffff0000000ull;
    offset = ~offset + 1;
    pc -= offset;
  } else
    pc += offset;

  // get the first adrp address
  pcc = pc;
  asm volatile(
    "ldr  w0, [%1];"
    "mov  %0, x0;"
    "lsr  %1, %1, #12;"
    "lsl  %1, %1, #12;"
    : "+r"(inst), "+r"(pc)
  );

  offset = (inst & 0xffffe0) << 9;
  offset |= (inst & 0x60000000) >> 17;
  if(offset >> 32) {
    offset |= 0xffffffff00000000ull;
    offset = ~offset + 1;
    pc -= offset;
  } else
    pc += offset;

  // get the actual address
  asm volatile(
    "ldr  w0, [%1, #4];"
    "mov  %0, x0;"
    : "+r"(inst) : "r"(pcc)
  );

  offset = ((inst & 0x3ffc00) >> 10) << 3;
  pc += offset;

  asm volatile(
    "str  %0, [%1];"   // replace the GOT entry
    : : "r"(fake), "r"(pc)
  );

}
