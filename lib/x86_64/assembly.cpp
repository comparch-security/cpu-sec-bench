#include "include/assembly.hpp"
#include <stdlib.h>

#define OTHER_OBJECT_GLOBARVAR

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

#ifdef DEBUG_OUTPUT
  std::cout << "instruction at the entry of rand(): " << std::hex << **((int **)pc) << std::endl; // this instruction should remain even with ASLR  
#endif
}

void replace_got_func(void **fake, void *got) {
  SET_MEM(got,fake);
}

#if defined(_MSC_VER)
  //use this func to mov target_register's val to rcx reg
  void func_to_modify_caller_parameter(int target_register){
    //fake use to supress optimization
    if(target_register < -256) target_register++;
  }

  x jum_target;
  CONTEXT sp_loc_context;
  uintptr_t target_offsets[10];
  int fake_use_arg = 0;
  //this func is used as the target func which is searched by bindump tools
  //so it is necessary to suppress compiler optimization toward it
  void labelfunc(int& fake_para){
    if(fake_para == 10) fake_para--; 
  }

#endif
