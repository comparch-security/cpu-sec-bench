#include "include/global_var.hpp"
#include "include/assembly.hpp"
#include <cstring>
#include <cstdlib>
#include <cstdint>

int ra_offset = sizeof(void*) * 16; //  Maximum height of stack data/ra copy
int ra_index = 0;
bool direct_exit = false;

void FORCE_NOINLINE helper(void* addr, void*ra_pos){
    volatile void *ra_stack = NULL;
    memmove(addr,(const void*)&ra_stack,ra_offset);
    for(int i = 0; i != ra_offset; i+=sizeof(void*)){
        if((uintptr_t)ra_pos == *(arch_uint_t*)(addr+i)){
            ra_index = i;
            break;
        }else{
            ra_stack = addr;
        }
    }
    return;
}

void FORCE_NOINLINE ra_target_func(void* addr_buffer ,int flutter_option){
    void* ra_pos = &&RA_POS;
    if(flutter_option == 1) goto *ra_pos;  // fake use

    COMPILER_BARRIER;
    helper(addr_buffer,ra_pos);
    COMPILER_BARRIER;
    // store the next inst in addr_buffer
RA_POS:
    if(direct_exit) exit(0);
    else gvar_decr();
    COMPILER_BARRIER;
    return;
}

int main(int argc, char** argv){

    int flutter_option = atoi(argv[1]);

    gvar_init(flutter_option);
    void* addr_buffer = malloc(ra_offset);

    COMPILER_BARRIER;
    ra_target_func(addr_buffer, flutter_option);
    COMPILER_BARRIER;

    void* jmp_target = malloc(sizeof(void*));
    memcpy(jmp_target, addr_buffer+ra_index, sizeof(void*));
    direct_exit = true;
    //goto *jmp_target;
    JMP_DAT_PTR(jmp_target);

    //if prog has run at this, it means the jump failed
    return gvar();
}