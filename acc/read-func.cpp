#include "include/assembly.hpp"
#include <fstream>
#include <sstream>
#include <string>
#include <cstdlib>

#include <iostream>

unsigned int code_num    = 0;
unsigned int code_mask   = 0;
unsigned int inst_byte_offset = 0;
/*
 * If a local variable is modified by an embedded assembly,
 * it might be removed by Clang 12 on Mac M1.
 *
 * When the targeted add (var += num) is enforced using volatile,
 * x86-64 might read the variable from the stack after the CHECK_POS label.
 *
 * On RV64GC, HiFive Unmatched, the GCC add a li assignment before the add.
 *
 * Consequently, we use a function to force the register assignement by ABI,
 * also set the sum using arguemnt to avoid imm assignemnt in RV64GC.
 * The add operation is plain C++ statement to avoid instruction removal
 * in Clang 12.
 *
 * When Intel CET is enabled,
 * the CHECK_POS returned by GCC might point to the endbr64 before var += cet
 * or correctly to var += cet.
 * Relax the check by reading both locations.
 */

int FORCE_NOINLINE helper(int var) {
  unsigned char *code = (unsigned char *)(&helper);
  code += inst_byte_offset;
  return ((*(unsigned int*)code) & code_mask) == code_num 
         ? 0 : 1;
}

int main(int argc, char* argv[]) {
  int var = argv[1][0] - '0';

  std::string fname = "./helper_fun.tmp";
  std::string cmd = "objdump -C --disassemble=\"helper(int)\" \
                    -j.text test/acc-read-func --show-raw-insn";
  std::string target = cmd + ">" + fname;

  int inst_th = 5;
  if(system(target.c_str()) == 0){
    std::ifstream tmpf(fname);
    if(tmpf.good()){
      std::string line;
      bool is_inst_data = false;

      while(std::getline(tmpf,line)){
        if(line.find("<helper(int)>") != std::string::npos){
          is_inst_data = true;
        }
        if(is_inst_data && inst_th != 0){
          inst_th--;
          std::istringstream firstopcode(line);
          std::getline(firstopcode,line,'\t');
          std::getline(firstopcode,line,'\t');
          firstopcode.str(line);
          
          if(inst_th != 0){
            int drop_num;
            while(firstopcode >> std::hex >> drop_num){
              inst_byte_offset++;
            }
          }else{
            int bytes_num = 0, tmp_num;
            while(firstopcode >> std::hex >> tmp_num){
              tmp_num <<= bytes_num;
              code_num |= tmp_num;code_mask |= (0xff << bytes_num);
              bytes_num += 8;
            }
          }
        }
      }
      std::cerr << "code_num is: " << std::hex << code_num << std::endl;
      std::cerr << "code_mask is: " << std::hex << code_mask << std::endl;
      std::cerr << "inst_byte_offset is: " << std::dec << inst_byte_offset << std::endl;
    }
  }
  else{
    return 2;
  }
  return helper(var);
}