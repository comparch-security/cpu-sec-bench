#include <cstdlib>
#include "include/global_var.hpp"
#include <string>

volatile arch_int_t offset;
int main(int argc, char* argv[]){

  INIT_TRACE_FILE;
  gvar_init(1);
  // get the offset of RA on stack
  if(argc == 2){
    std::string cmd_offset = argv[1];
    offset = 4 * stoll(cmd_offset);
  }else{
    std::string cmd_offset = argv[1];
    std::string extra_offset = argv[2];
    offset = 4*(stoll(cmd_offset) + stoll(extra_offset));
  }
  void *ret_label = (void*)&main;
  GET_LABEL_ADDRESS(ret_label,TARGET_LABEL);
  if(offset == -1) { GOTO_SAVED_LABEL(ret_label);}   // impossible to happen

  // call a function but illegally return
  assembly_helper(ret_label);
  gvar_decr();
  COMPILER_BARRIER;
  // the elligal return site
TARGET_LABEL(argc)
  gvar_decr();
  exit(gvar());
}