// assembly helper functions
// x86_64

// declare a label in assembly
#define DECL_LABEL(label) asm volatile(#label ":")

// modify stack
#define MOD_STACK_LABEL(label, offset)                  \
  asm volatile(                                         \
               "movq %%rsp, %%rax;"                     \
               "addq  %0, %%rax;"                       \
               "movq $" #label ", (%%rax);"             \
               : : "r"(offset)                          \
               : "rax"                                  \
                                                        )

#define MOD_STACK_DAT(dat, offset)                      \
  asm volatile(                                         \
               "movq %%rsp, %%rax;"                     \
               "addq  %1, %%rax;"                       \
               "movq %0, (%%rax);"                      \
               : : "r"(dat), "r"(offset)                \
               : "rax"                                  \
                                                        )

// detect the stack
extern unsigned long long min_stack_size;
extern void asm_stack_test();


// modify return address to a label
#define MOD_RET_LABEL(label) MOD_STACK_LABEL(label, min_stack_size)
#define MOD_RET_DAT(dat)     MOD_STACK_DAT(dat, min_stack_size)

// exchange memory value
#define XCHG_MEM(ptrL, ptrR)   \
  asm volatile(                \
    "movq (%0), %%rax;"        \
    "xchg %%rax, (%1);"        \
    "movq %%rax, (%0);"        \
    : : "r" (ptrL), "r" (ptrR) \
    : "rax"                    \
                               )


// call a function
#define CALL_FUNC(pFunc) \
  asm volatile(          \
    "call *%0;"          \
    : : "r" (pFunc)      \
                         )

// pass a integer argument
#define PASS_INT_ARG(Idx, arg) PASS_INT_ARG##Idx(arg)

#define PASS_INT_ARG0(arg) asm volatile("mov %0, %%rdi;" : : "r" (arg))
#define PASS_INT_ARG1(arg) asm volatile("mov %0, %%rsi;" : : "r" (arg))

// pass a double argument
#define PASS_DOUBLE_ARG_FROM_INT(Idx, arg) \
  asm volatile(                            \
    "movq %0, %%rax;"                      \
    "movq %%rax, %%xmm" #Idx ";"           \
    : : "r" (arg)                          \
    : "rax", "xmm" #Idx                    \
                                           )

// create a fake return stack
#define PUSH_FAKE_RET_LABEL(label)      \
  asm volatile(                         \
    "push " #label "@GOTPCREL(%%rip);"  \
    "subq %0, %%rsp;"                   \
    : : "r"(min_stack_size)             \
                                        )

// push an address
#define PUSH_LABEL(label) asm volatile("push " #label "@GOTPCREL(%rip)")


// return
#define RET asm volatile("ret")

//call to a label
#define CALL_LABEL(label, offset)            \
  asm volatile(                              \
    "mov " #label "@GOTPCREL(%%rip), %%rax;" \
    "add %0, %%rax;"                         \
    "call *%%rax;"                           \
    : : "i"(offset) : "rax"                  \
                                             )

// jump to a label
#define JMP_LABEL(label, offset)             \
  asm volatile(                              \
    "mov " #label "@GOTPCREL(%%rip), %%rax;" \
    "add %0, %%rax;"                         \
    "jmp *%%rax;"                            \
    : : "i"(offset) : "rax"                  \
                                             )

// a instrction that can jmp to the middle
// 48 05 c3 00 00 00    	add    $0xc3,%rax
// c3 retq
// offset = 2
#define MID_INSTRUTION asm volatile("mid_instruction: add $0xc3, %%rax;" : : : "rax")

#define POP_STACK asm volatile("pop %%rax" : : : "rax")

// the machine code of a function
//
//  unsigned int func() {
//    return 0;
//  }
//  00:       31 c0                   xor    %eax,%eax
//  02:       c3                      retq
#define FUNC_MACHINE_CODE {0x31, 0xc0, 0xc3}
