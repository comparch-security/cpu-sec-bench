#include "include/global_var.hpp"
#include "include/assembly.hpp"
#include <cstring>
#include <cstdlib>
#include <cstdint>

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

void FORCE_NOINLINE helper(void* addr, void*ra_pos){
    volatile void *ra_stack = NULL;
    memmove(addr,(const void*)&ra_stack,ra_offset);
    for(int i = 0; i != ra_offset; i+=sizeof(void*)){
        if((uintptr_t)ra_pos == *(arch_uint_t*)((long long)addr+(long long)i)){
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
    global_ra_pos = &&RA_POS;
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
                direct_exit = true;
                //goto *jmp_target;
                JMP_DAT_PTR(jmp_target);
                //if prog has run at this, it means the jump failed
            }
            return gvar();
            break;
        default:
            break;
    }

    return 1;
}