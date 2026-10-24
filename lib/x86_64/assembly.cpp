#define INTERN_RELATED_GLOBALVAR
#define TRACE_EXTERN_RELATED_GLOBALVAR
#include "include/assembly.hpp"
#include <stdlib.h>

void get_got_func(void **gotp, void *label, int cet) {
  char *pc = (char *)label;

  // the call site may carry an endbr64 prefix when CET is enabled
  if((unsigned char)pc[0] == 0xf3 && (unsigned char)pc[1] == 0x0f &&
     (unsigned char)pc[2] == 0x1e && (unsigned char)pc[3] == 0xfa) pc += 4;

  // require the "e8 rel32" call: an unrecognised instruction shape reports a
  // failure to the caller instead of dereferencing a pointer we never verified
  if((unsigned char)pc[0] != 0xe8) { *gotp = NULL; return; }
  int offset = *(int *)(pc+1);
  pc += 5 + offset; // point to 1st instruction in plt

  // the stub's "ff 25 rel32" jump may carry an endbr64 or a bnd prefix
  for(;;) {
    if((unsigned char)pc[0] == 0xf3 && (unsigned char)pc[1] == 0x0f &&
       (unsigned char)pc[2] == 0x1e && (unsigned char)pc[3] == 0xfa) { pc += 4; continue; }
    if((unsigned char)pc[0] == 0xf2) { pc += 1; continue; }
    break;
  }
  if((unsigned char)pc[0] != 0xff || (unsigned char)pc[1] != 0x25) { *gotp = NULL; return; }
  offset = *(int *)(pc+2);
  pc += 6 + offset;

  *gotp = pc;

#ifdef DEBUG_OUTPUT
  std::cout << "instruction at the entry of rand(): " << std::hex << **((int **)pc) << std::endl; // this instruction should remain even with ASLR  
#endif
}

void replace_got_func(void **fake, void *got) {
  SET_MEM(got,fake);
}

#if defined(_MSC_VER)
  //use this func to mov target_register's val to rcx reg
  FORCE_NOINLINE void func_to_modify_caller_parameter(int target_register){
    //fake use to supress optimization
    if(target_register < -256) target_register++;
  }

  void get_label(void* &ptr){
    static int index = 0;
    uintptr_t offset = -1;
    //read temp file once
    if(index == 0){
      std::ifstream offset_tmp("offset.tmp");
      if(offset_tmp.good()){
        int i = 0;
        while(offset_tmp >> offset){
          #ifdef DEBUG_OUTPUT
          if(offset == -1){
            std::cerr << "offset tmp file is empty" << std::endl;
          }
          #endif
          target_offsets[i++] = offset;
        }
      }
      #ifdef DEBUG_OUTPUT
      else{
        std::cerr << "offset tmp file is not exist" << std::endl;
      }
      #endif
      offset_tmp.close();
    }
    //get one offset from arry
    ptr = (void*)((uintptr_t)ptr + target_offsets[index++]);
  }

  //this func is used as the target func which is searched by bindump tools
  //so it is necessary to suppress compiler optimization toward it
  FORCE_NOINLINE void labelfunc(){
  }
#endif
