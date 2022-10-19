#include "include/assembly.hpp"
#include "include/temp_file.hpp"
#include <fstream>
#include <sstream>
  
int main(int argc, char* argv[]){

  int inst_th = 5;

  std::string proc_name = "./test/acc-read-func";
  std::string func_name = "helper";
  std::string file_name = "./test/helper_fun.tmp";

  unsigned int code_num;
  unsigned int code_mask;
  unsigned int inst_byte_offset;

  if(run_dump_cmd(proc_name,func_name,file_name) == 0){
    if(get_nth_func_inst(code_num,code_mask,inst_byte_offset,file_name,inst_th) == 0){

      write_to_temp_file(code_num, argc, argv);
      write_to_temp_file(code_mask, argc, argv);
      write_to_temp_file(inst_byte_offset, argc, argv);
    }else return 2;
  }else return 2;
  return 64;
}