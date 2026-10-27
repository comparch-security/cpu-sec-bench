#include "include/global_var.hpp"
#include "include/assembly.hpp"
#include <cstring>
#include <cstdlib>
#include <cstdint>
#include <iostream>

int ra_offset = sizeof(void*) * 16; //  Maximum height of stack data/ra copy
int ra_index = 0;
bool direct_exit = false;
/*volatile modifier can not be removed,
 *otherwise will cause segment fault in riscv/gcc
 *(jump again will interrupt s0 register in ra_target_func frame)
 *(Oddly, same ra_target_func function in XXX-implicit-arith.cpp and
 * XXX-explicit-arith.cpp, this test was not influenced,
 * but XXX-implicit-arith.cpp was influenced)
*/
volatile void* global_ra_pos = NULL;

/* The landed block must not read a register another function's prologue set: on
 * riscv64 the compiler keeps &direct_exit in s0, and the jump arrives with main's
 * s0 (the heap buffer), so the in-block read would fetch a heap byte instead of
 * the flag.  Reading it inside a function of its own materializes the address
 * where the read happens.
 */
bool FORCE_NOINLINE read_direct_exit() {
    return direct_exit;
}

void FORCE_NOINLINE helper(void* addr, void*ra_pos){
    volatile void *ra_stack = NULL;
    memmove(addr,(const void*)&ra_stack,ra_offset);
    for(int i = 0; i != ra_offset; i+=sizeof(void*)){
        arch_uint_t *slot = (arch_uint_t*)((long long)addr+(long long)i);
        if((uintptr_t)ra_pos == (uintptr_t)STRIP_PAC_VAL(*slot)){
            *slot = (arch_uint_t)STRIP_PAC_VAL(*slot);
            ra_index = i;
            break;
        }else{
            ra_stack = addr;
        }
    }
    return;
}

void FORCE_NOINLINE ra_target_func(void* addr_buffer ,int flutter_option){
    void* ra_pos;
    GET_LABEL_ADDRESS(ra_pos,TARGET_LABEL);
    global_ra_pos = ra_pos;
    if(flutter_option == 1) { GOTO_SAVED_LABEL(ra_pos);} // fake use

    COMPILER_BARRIER;
    helper(addr_buffer,ra_pos);
    COMPILER_BARRIER;
    // store the next inst in addr_buffer
TARGET_LABEL(flutter_option)
    if(read_direct_exit()) exit(0);
    else gvar_decr();
    COMPILER_BARRIER;
    return;
}

int main(int argc, char** argv){

    INIT_TRACE_FILE;
    int flutter_option = atoi(argv[1]);
    int flag_option    = atoi(argv[2]);

    gvar_init(flutter_option);
    void* addr_buffer = malloc(ra_offset);

    COMPILER_BARRIER;
    ra_target_func(addr_buffer, flutter_option);
    COMPILER_BARRIER;

    void* jmp_target = malloc(sizeof(void*));
    memcpy(jmp_target, (void*)((long long)addr_buffer+(long long)ra_index), 
    sizeof(void*));

    switch(flag_option){
        case 1:
            if(*(uintptr_t*)jmp_target == (uintptr_t)global_ra_pos)
                return 0;
            break;
        case 2:
            //this conditional statement is used to bypass M1/clang jump check
            //M1 forbidden jump directly from switch-case;
            if(!direct_exit){
                // jump only when the heap copy really holds the block address:
                // a copy that did not happen must deliver the declared 1 below
                // instead of jumping to whatever the heap held
                if(*(uintptr_t*)jmp_target != (uintptr_t)global_ra_pos)
                    break;
                direct_exit = true;
                //goto *jmp_target;
                JMP_DAT_PTR(jmp_target);
                WRITE_TRACE("Successful Jumped", "");
                //if prog has run at this, it means the jump failed
            }
            return 1; // the jump did not land
            break;
        default:
            break;
    }
    free(jmp_target);
    return 1;
}