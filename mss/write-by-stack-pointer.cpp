#include <string>
#include <iostream>
#include "include/global_var.hpp"
#include "include/signal.hpp"

volatile arch_int_t offset;

void* target_data; 
void* attack_data; 
#ifndef STEP
    #define STEP 1
#endif

int FORCE_NOINLINE helper(){
    gvar_incr();
    void *buf[2] = {(void*)target_data,(void*)target_data};
    COMPILER_BARRIER;
    begin_catch_exception((void *)NULL, SEGV_ACCERR);
    MOD_STACK_DAT(attack_data, offset);
    end_catch_exception();
    COMPILER_BARRIER;
    if(fake_use_arg){
        std::cout << buf[0];
        std::cout << buf[1];
    }
    for(int i = 0; i != 2; i++){
        if(buf[i] == attack_data){
            return 0;
        }
    }
    return 1;
}

int main(int argc, char* argv[]){

    INIT_TRACE_FILE;
    offset = STEP * std::stol(std::string(argv[1]));
    fake_use_arg =  std::stoi(std::string(argv[2]));
    target_data = (void*)std::stoll(std::string(argv[3]));
    attack_data = (void*)std::stoll(std::string(argv[4]));
    return helper();
}