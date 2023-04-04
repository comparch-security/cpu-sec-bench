#include "include/global_var.hpp"
#include "include/assembly.hpp"
#include <cstring>
#include <cstdlib>
#include <cstdint>

bool direct_exit = false;

void FORCE_NOINLINE leaf_func(void* addr, const void* ra_stack){
    memcpy(addr,ra_stack,sizeof(void*));
}

#define RA_POS_TEST_ENTRY(off,rv)           \
  READ_STACK_DAT_IMM(ra_stack, off);        \
  if(ra_stack == ra_label) {                \
    leaf_func(addr,(const void*)&ra_stack); \
    return; }                               \


#define RA_POS_TEST          \
  RA_POS_TEST_ENTRY(0, 0)    \
  RA_POS_TEST_ENTRY(8, 2)    \
  RA_POS_TEST_ENTRY(16, 4)   \
  RA_POS_TEST_ENTRY(24, 6)   \
  RA_POS_TEST_ENTRY(32, 8)   \
  RA_POS_TEST_ENTRY(40, 10)  \
  RA_POS_TEST_ENTRY(48, 12)  \
  RA_POS_TEST_ENTRY(56, 14)  \
  RA_POS_TEST_ENTRY(64, 14)  \

void FORCE_NOINLINE helper(void* addr, void*ra_label){
    volatile void *ra_stack = NULL;

    gvar_incr();
    RA_POS_TEST;
    gvar_init(-2);
    return;
}

void FORCE_NOINLINE ra_target_func(void* addr_buffer ,int flutter_option){
    void* ra_pos = &&RA_POS;
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

    gvar_init(flutter_option);
    void* addr_buffer = malloc(sizeof(void*));

    COMPILER_BARRIER;
    ra_target_func(addr_buffer,flutter_option);
    COMPILER_BARRIER;
    // store the next inst in addr_buffer

    if(!direct_exit){
        direct_exit = true;
        JMP_DAT_PTR(addr_buffer);
    }
    return gvar();
}