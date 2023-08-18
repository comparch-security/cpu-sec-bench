#include "include/global_var.hpp"
#include "include/assembly.hpp"
#include <cstring>
#include <cstdlib>
#include <cstdint>

bool direct_exit = false;
/*volatile modifier can not be removed,
 *otherwise will cause segment fault in riscv/gcc
 *(jump again will interrupt s0 register in ra_target_func frame)
*/
volatile void* global_ra_pos = NULL;

void FORCE_NOINLINE leaf_func(void* addr, const void* ra_stack){
    memcpy(addr,ra_stack,sizeof(void*));
}

#define RA_POS_TEST_ENTRY(off,rv)           \
  READ_STACK_DAT_IMM(ra_stack, off);        \
  if(ra_stack == ra_label) {                \
    leaf_func(addr,(const void*)&ra_stack); \
    return; }                               \

void FORCE_NOINLINE helper(void* addr, void*ra_label){
    volatile void *ra_stack = NULL;

    gvar_incr();
    RA_POS_TEST;
    gvar_init(-2);
    return;
}

void FORCE_NOINLINE ra_target_func(void* addr_buffer ,int flutter_option){
    void* ra_pos = &&RA_POS;
    global_ra_pos = ra_pos;
    if(flutter_option == 1) goto *ra_pos;  // fake use

    COMPILER_BARRIER;
    helper(addr_buffer,ra_pos);
    COMPILER_BARRIER;
RA_POS:
    if(direct_exit) exit(0);
    else gvar_incr();
    COMPILER_BARRIER;
    return;
}

int main(int argc, char** argv){

    int flutter_option = atoi(argv[1]);
    //To avoid setting exit entry in the lr register in Mac M1 clang
    direct_exit = (bool) (argv[2][0] - '0');
    int flag_option = atoi(argv[3]);

    gvar_init(flutter_option);
    void* addr_buffer = malloc(sizeof(void*));

    COMPILER_BARRIER;
    ra_target_func(addr_buffer,flutter_option);
    COMPILER_BARRIER;
    // store the next inst in addr_buffer

    switch(flag_option){
        case 1:
            if(*(uintptr_t*)addr_buffer == (uintptr_t)global_ra_pos)
                free(addr_buffer);
                return 0;
            break;
        case 2:
            //this conditional statement is used to bypass M1/clang jump check
            //M1 forbidden jump directly from switch-case;
            if(!direct_exit){
                direct_exit = true;
                //goto *jmp_target;
                JMP_DAT_PTR(addr_buffer);
                //if prog has run at this, it means the jump failed
                free(addr_buffer);
                return gvar();
            }
            break;
        default:
            break;
    }

    return 1;
}