#include "include/assembly.hpp"
#include <stdlib.h>
#include <sstream>
#include <fstream>

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

int run_dump_cmd(const std::string& procname,
                 const std::string& funcname, const std::string& filename){
  std::string cmd = "./script/get_x86_func_inst.sh " + procname + " "
                    + funcname + " " + filename;
  return system(cmd.c_str());
}

int get_nth_func_inst(unsigned int& num,unsigned int& mask,
                       unsigned int& offset,const std::string name, int nth){
  num = mask = offset = 0;
  std::ifstream tmpf(name);
  if(tmpf.good()){
    std::string line;
    while(nth--){
      std::getline(tmpf,line);
      std::istringstream firstopcode(line);

      if(nth != 0){
        int drop_num;
        while(firstopcode >> std::hex >> drop_num){
          offset++;
        }
      }else{
        int bytes_num = 0, tmp_num;
        while(firstopcode >> std::hex >> tmp_num){
          tmp_num <<= bytes_num;
          num |= tmp_num;mask |= (0xff << bytes_num);
          bytes_num += 8;
        }
      }
    }
    return 0;
  }else return 2;
}