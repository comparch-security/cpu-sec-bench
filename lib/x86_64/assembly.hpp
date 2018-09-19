// assembly helper functions
// x86_64

// declare a label in assembly
#define DECL_LABEL(label) asm volatile(#label ":")

// jump to a label
#define JMP_LABEL(label) asm volatile("jmp " #label ";")

// modify stack to a label
#define MOD_STACK_LABEL(label, offset) \
  asm volatile("movq $" #label ", " #offset "(%rsp);")

// modify return address to a label
#ifdef STACK_FP_RET
  #define MOD_RET_LABEL(label) MOD_STACK_LABEL(label, 8)
#elif defined(STACK_RET)
  #define MOD_RET_LABEL(label) MOD_STACK_LABEL(label, 0)
#endif

// assembly anchor
#define ASM_ANCHOR asm volatile("nop;")
