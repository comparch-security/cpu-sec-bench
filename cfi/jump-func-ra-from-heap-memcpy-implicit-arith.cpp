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

/* The landed block must not read a register another function's prologue set: on
 * riscv64 the compiler keeps &global_ra_pos in s0, and the jump arrives with
 * main's s0 (argv), so the in-block read would fetch a zero byte, skip the exit
 * and run the block's epilogue on main's frame.  Reading the flag inside a
 * function of its own materializes the address where the read happens.
 */
bool FORCE_NOINLINE read_direct_exit() {
    return direct_exit;
}

void FORCE_NOINLINE leaf_func(void* addr, const void* ra_stack){
    memcpy(addr,ra_stack,sizeof(void*));
}

#define RA_POS_TEST_ENTRY(off,rv)           \
  READ_STACK_DAT_IMM(ra_stack, off);        \
  if(STRIP_PAC(ra_stack) == ra_label) {     \
    ra_stack = STRIP_PAC(ra_stack);         \
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
    void* ra_pos;
    GET_LABEL_ADDRESS(ra_pos,TARGET_LABEL);
    global_ra_pos = ra_pos;
    if(flutter_option == 1) { GOTO_SAVED_LABEL(ra_pos);} // fake use

    COMPILER_BARRIER;
    helper(addr_buffer,ra_pos);
    COMPILER_BARRIER;
TARGET_LABEL(flutter_option)
    if(read_direct_exit()) exit(0);
    else gvar_incr();
    COMPILER_BARRIER;
    return;
}

int main(int argc, char** argv){

    INIT_TRACE_FILE;
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
            if(*(uintptr_t*)addr_buffer == (uintptr_t)global_ra_pos) {
                free(addr_buffer);
                return 0;
            }
            break;
        case 2:
            //this conditional statement is used to bypass M1/clang jump check
            //M1 forbidden jump directly from switch-case;
            if(!direct_exit){
                // jump only when the heap copy really holds the block address:
                // a copy that did not happen must deliver the declared 1 below
                // instead of jumping to whatever the heap held
                if(*(uintptr_t*)addr_buffer != (uintptr_t)global_ra_pos)
                    break;
                direct_exit = true;
                //goto *jmp_target;
                JMP_DAT_PTR(addr_buffer);
                //if prog has run at this, it means the jump failed
                WRITE_TRACE("Successful Jumped", "");
                free(addr_buffer);
                return 1; // the jump did not land
            }
            break;
        default:
            break;
    }

    return 1;
}