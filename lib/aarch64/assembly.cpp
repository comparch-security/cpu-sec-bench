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

#ifdef COMPILER_GCC
void get_got_func(void **gotp, void *label, int cet) {
  char *pc = (char *)label;
  arch_int_t offset = 0;

#ifdef DEBUG_READ_GOT
  std::cout << "label to rand(): " << std::hex << (unsigned long long)pc << " " << offset << std::endl;
#endif

  // assembly:
  //  a58:       97ffffc2        bl      960 <rand@plt>
  //
  // bl: 100101, imm26
  // pc = pc + imm26 * 4

  offset = (*(int *)pc) & 0x03ffffff; // get the imm26
  if(offset & 0x3000000) offset |= 0xfffffffffc000000llu; // sign extended to 64
  offset *= 4;
  pc += offset;

#ifdef DEBUG_READ_GOT
  std::cout << "pos of dyld stub: " << std::hex << (unsigned long long)pc << " " << offset << std::endl;
#endif

  // assembly
  //   990:   b0000090        adrp    x16, 11000 <__FRAME_END__+0xfab0>
  //   994:   f947ba11        ldr     x17, [x16, #3952]
  //   998:   913dc210        add     x16, x16, #0xf70
  //   99c:   d61f0220        br      x17
  //
  //  adrp: 1, imm2, 10000, imm19, rd5;  {imm19,imm2} -> imm21
  //  pc = pc & 0xfffff000 + ext(imm21 << 12)
  //  ldr: 1111100101, imm12, rs5, rd5
  //  pc = pc + ext(imm12 * 4)
  int instr_adrp = *(int *)pc;
  int instr_ldr  = *(int *)(pc+4);

  offset = (arch_int_t)pc;
  offset &= 0xfffffffffffff000;
  pc = (char *)offset;

  offset = ((instr_adrp >> 3) & 0x01ffffc) | ((instr_adrp >> 29) & 0x03); // get the imm21
  if(offset & 0x100000) offset |= 0xfffffffffff00000llu; // sign extended to 64
  offset *= 4096;
  pc += offset;

  offset = (instr_ldr >> 10) & 0x0fff;
  if(offset & 0x0800) offset |= 0xfffffffffffff800; // sign extension
  offset *= 8;
  pc += offset;

#ifdef DEBUG_READ_GOT
  std::cout << "pos of GOT entry: " << std::hex << (unsigned long long)pc << " " << offset << std::endl;
#endif

  *gotp = pc;

#ifdef DEBUG_READ_GOT
  std::cout << "instruction at the entry of rand(): " << std::hex << **((int **)pc) << std::endl; // this instruction should remain even with ASLR
#endif
}

#endif

#ifdef COMPILER_CLANG
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
  offset = (*(int *)pc) & 0x03ffffff; // get the imm26
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
  offset = (*(int *)pc >> 5) & 0x07ffff; // get the imm19
  if(offset & 0x40000) offset |= 0xfffffffffff80000llu; // sign extended to 64
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
#endif

void replace_got_func(void **fake, void *got) {
  asm volatile(
    "str  %0, [%1];"   // replace the GOT entry
    : : "r"(fake), "r"(got)
  );

}

int run_dump_cmd(const std::string& procname,
                 const std::string& funcname, const std::string& filename){
  std::string cmd = "./script/get_aarch64_func_inst.sh " + procname + " "
                    + funcname + " " + filename;
  return system(cmd.c_str());
}