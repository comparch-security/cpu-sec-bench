#include "include/assembly.hpp"
#include <stdlib.h>

int dummy_leaf_rv = 0;

int FORCE_NOINLINE dummy_leaf_func(int v) {
  return v + rand();
}

// we know rand is call immediately afterwards
// so get the plt address from ra
#define GET_GOT_LOC                    \
  asm volatile(                        \
    "mov  %0, x30;"                    \
    "ldr  w8, [x30];"                  \
    "mov  %1,x8;"                      \
    : "+r"(pc), "+r"(inst) : : "x8"    \
  );                                   \
  offset = (inst & 0x03ffffff) << 2;   \
  if(offset & 0x08000000) {            \
    offset |= 0xfffffffff0000000ull;   \
    offset = ~offset + 1;              \
    pc -= offset;                      \
  } else                               \
    pc += offset;                      \
  pcc = pc;                            \
  asm volatile(                        \
    "ldr  w8, [%1];"                   \
    "mov  %0, x8;"                     \
    "lsr  %1, %1, #12;"                \
    "lsl  %1, %1, #12;"                \
    : "+r"(inst), "+r"(pc) : : "x8"    \
  );                                   \
  offset = (inst & 0xffffe0) << 9;     \
  offset |= (inst & 0x60000000) >> 17; \
  if(offset >> 32) {                   \
    offset |= 0xffffffff00000000ull;   \
    offset = ~offset + 1;              \
    pc -= offset;                      \
  } else                               \
    pc += offset;                      \
  asm volatile(                        \
    "ldr  w8, [%1, #4];"               \
    "mov  %0, x8;"                     \
    : "+r"(inst) : "r"(pcc) : : "x8"   \
  );                                   \
  offset = ((inst & 0x3ffc00) >> 10) << 3; \
  pc += offset;                        \

void get_got_func(void **gotp) {
  char *pc = NULL, *pcc = NULL;
  unsigned int inst = 0;
  unsigned long long offset;

  GET_GOT_LOC

  *gotp = pc;
}

void replace_got_func(void **fake) {
  char *pc = NULL, *pcc = NULL;
  unsigned int inst = 0;
  unsigned long long offset;

  GET_GOT_LOC

  asm volatile(
    "str  %0, [%1];"   // replace the GOT entry
    : : "r"(fake), "r"(pc)
  );

}

#undef GET_GOT_LOC
