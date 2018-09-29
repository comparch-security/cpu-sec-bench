// assembly helper functions
// x86_64

// declare a label in assembly
#define DECL_LABEL(label) asm volatile(#label ":")

// jump to a label
#define JMP_LABEL(label) asm volatile("jmp " #label ";")

// modify stack
#define MOD_STACK_LABEL(label, offset) \
  asm volatile("movq $" #label ", " #offset "(%rsp);")
#define MOD_STACK_DAT(dat, offset) \
  asm volatile("movq %0, " #offset "(%%rsp);" : : "r" (dat))

// modify return address to a label
#ifdef STACK_FP_RET
  #define MOD_RET_LABEL(label) MOD_STACK_LABEL(label, 8)
  #define MOD_RET_DAT(dat)     MOD_STACK_DAT(dat, 8)
#elif defined(STACK_RET)
  #define MOD_RET_LABEL(label) MOD_STACK_LABEL(label, 0)
  #define MOD_RET_DAT(dat)     MOD_STACK_DAT(dat, 0)
#endif

// exchange memory value
#define XCHG_MEM(ptrL, ptrR)   \
  asm volatile(                \
    "movq (%0), %%rax;"        \
    "xchg %%rax, (%1);"        \
    "movq %%rax, (%0);"        \
    : : "r" (ptrL), "r" (ptrR) \
    : "rax"                    \
  )                            \


// call a function
#define CALL_FUNC(pFunc) \
  asm volatile(          \
    "call *%0;"          \
    : : "r" (pFunc)      \
  )                      \

// pass a integer argument
#define PASS_INT_ARG(Idx, arg) PASS_INT_ARG##Idx(arg)

#define PASS_INT_ARG0(arg) asm volatile("mov %0, %%rdi;" : : "r" (arg))
#define PASS_INT_ARG1(arg) asm volatile("mov %0, %%rsi;" : : "r" (arg))

// pass a double argument
#define PASS_DOUBLE_ARG_FROM_INT(Idx, arg) \
  asm volatile(                  \
    "movq %0, %%rax;"            \
    "movq %%rax, %%xmm" #Idx ";" \
    : : "r" (arg)                \
    : "rax", "xmm" #Idx          \
  )                              \

