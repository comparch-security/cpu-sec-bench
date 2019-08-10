#include "include/assembly.hpp"

unsigned long long min_stack_size = 0;
unsigned long long helper_sp = 0;

void FORCE_NOINLINE asm_stack_test_helper() {
	  //asm volatile("ld %0,0(sp);" : "=r"(helper_sp) : : );
	  asm volatile("mv %0,sp;" : "=r"(helper_sp) : : );
       int a=0;
      static int b = 2;
}

void  asm_stack_test() {
	  //asm volatile("ld %0,0(sp);" : "=r"(min_stack_size) : : );
      asm volatile("mv %0,sp;" : "=r"(min_stack_size) : : );

  asm_stack_test_helper();
  //min_stack_size -= (helper_sp + 8);
 min_stack_size -=helper_sp;
}

void replace_got_func(void **org, void **fake) {
}

