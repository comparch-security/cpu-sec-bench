#include <string>
#include "include/global_var.hpp"
#include "include/signal.hpp"

volatile arch_int_t offset;

#define TARGET_DATA 0x0000000000000000ULL
#define ATTACK_DATA 0xffffffffffffffffULL
#ifndef STEP
    #define STEP 4
#endif

void* fake_use_buffer[2];

int FORCE_NOINLINE helper(){
    gvar_incr();
    void *buf[2] = {TARGET_DATA,TARGET_DATA};
    COMPILER_BARRIER;
    begin_catch_exception((void *)NULL, SEGV_ACCERR);
    MOD_STACK_DAT(ATTACK_DATA, offset);
    end_catch_exception();
    COMPILER_BARRIER;
    if(fake_use_arg){
        fake_use_buffer[0] = buf[0];
        fake_use_buffer[1] = buf[1];
    }
    for(int i = 0; i != 2; i++){
        if((unsigned long long)buf[i] == ATTACK_DATA){
            return 0;
        }
    }
    return 1;
}

int main(int argc, char* argv[]){

    INIT_TRACE_FILE;
    offset = STEP * std::stol(std::string(argv[1]));
    return helper();
}