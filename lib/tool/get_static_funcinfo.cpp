#include "include/assembly.hpp"
#include "include/temp_file.hpp"
#include <fstream>
#include <sstream>

/* Get func's nth inst-info of prog and output it to ofile*/
int get_func_nth_inst(std::string prog, std::string func, 
                      std::string ofile, int nth){
  
  unsigned int code_num = 0;
  unsigned int code_mask = 0;
  unsigned int inst_byte_offset = 0;

  std::string dump_file = "./test/get_func_nth_inst.tmp";
  if(run_dump_cmd(prog,func,dump_file) == 0){//call arch related shell code
    std::ifstream itmpf(dump_file);
    if(itmpf.good()){
      std::string line;
      int tmp_num, bytes_num = 0;
      while(nth--){
        std::getline(itmpf,line);
        std::istringstream sline(line);
        if(nth != 0){//handle 1-(n-1)th inst, count these inst byte num
          while(sline >> std::hex >> tmp_num) inst_byte_offset++;
        }else{//handle inst opcode mode is the little endian 
          while(sline >> std::hex >> tmp_num){
            tmp_num <<= bytes_num;
            code_num |= tmp_num;code_mask |= (0xff << bytes_num);
            bytes_num += 8;
          }
        }
      }
      std::ofstream otmpf(ofile);
      if(otmpf.good()){
        otmpf << code_num << "\n";
        otmpf << code_mask << "\n";
        otmpf << inst_byte_offset;
        otmpf.close();
        return 0;
      }
    }
  }
  return -1;
}