#include "include/global_var.hpp"
#include "include/assembly.hpp"
#include <cstring>
#include <cstdlib>
#include <cstdint>

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

int main(int argc, char** argv){

    int flutter_option = atoi(argv[1]);

    gvar_init(flutter_option);
    void* addr_buffer = malloc(sizeof(void*));
    void* ra_pos = &&RA_POS;
    if(flutter_option == 1) goto *ra_pos;  // fake use
    COMPILER_BARRIER;
    helper(addr_buffer,ra_pos);
    COMPILER_BARRIER;
    // store the next inst in addr_buffer
RA_POS:
    gvar_decr();
    COMPILER_BARRIER;

    if(gvar() > 0){
        while(gvar()) JMP_DAT_PTR(addr_buffer);
    }

    return gvar();
}