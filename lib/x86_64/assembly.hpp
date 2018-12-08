// assembly helper functions
// x86_64

// declare a label in assembly
#define DECL_LABEL(label) \
  asm volatile(#label ":")

// modify return address to a label
#define MOD_RET_LABEL(label) \
  asm volatile( "movq $" #label ", 8(%rbp);" )
#define MOD_RET_DAT(dat)     \
  asm volatile( "movq %0, 8(%%rbp);" : : "r"(dat) )

// get the address of the return address using a gcc builtin
// NOTE FOR POSSIBLE SIDE-EFFECT
//   If this macro is used, the compiler is forced to push %rbp on the stack,
// which is equivalent to enforce gcc argument -fno-omit-frame-pointer option
// on the current frame.
#define GET_RET_ADDR \
  ((void **)__builtin_frame_address(0) + 1)

// exchange memory value
#define XCHG_MEM(ptrL, ptrR)          \
  asm volatile(                       \
    "movq (%0), %%rax;"               \
    "xchg %%rax, (%1);"               \
    "movq %%rax, (%0);"               \
    : : "r" (ptrL), "r" (ptrR)        \
    : "rax"                           \
                                      )


// call a function
#define CALL_FUNC(pFunc)        \
  asm volatile(                 \
    "call *%0;"                 \
    : : "r" (pFunc)             \
                                )

// pass a integer argument
#define PASS_INT_ARG(Idx, arg) \
  PASS_INT_ARG##Idx(arg)

#define PASS_INT_ARG0(arg) \
  asm volatile("mov %0, %%rdi;" : : "r" (arg))
#define PASS_INT_ARG1(arg) \
  asm volatile("mov %0, %%rsi;" : : "r" (arg))

// pass a double argument
#define PASS_DOUBLE_ARG_FROM_INT(Idx, arg)        \
  asm volatile(                                   \
    "movq %0, %%rax;"                             \
    "movq %%rax, %%xmm" #Idx ";"                  \
    : : "r" (arg)                                 \
    : "rax", "xmm" #Idx                           \
                                                  )

// create a fake return stack
#define PUSH_FAKE_RET(label)                 \
  asm volatile(                              \
    "lea " #label "(%%rip), %%rax;"          \
    "push %%rax;"                            \
    "push %0;"                               \
    : : "r" (__builtin_frame_address(0))     \
    : "rax"                                  \
                                             )


// push an address
#define PUSH_LABEL(label)                    \
  asm volatile(                              \
    "lea " #label "(%%rip), %%rax;"          \
    "push %%rax;"                            \
    : : : "rax"                              \
                                             )


// return
#define RET \
  asm volatile("ret")

//call to a label
#define CALL_LABEL(label, offset)            \
  asm volatile(                              \
    "lea " #label "(%%rip), %%rax;"          \
    "add %0, %%rax;"                         \
    "call *%%rax;"                           \
    : : "i"(offset) : "rax"                  \
                                             )

// jump to a label
#define JMP_LABEL(label, offset)             \
  asm volatile(                              \
    "lea " #label "(%%rip), %%rax;"          \
    "add %0, %%rax;"                         \
    "jmp *%%rax;"                            \
    : : "i"(offset) : "rax"                  \
                                             )

// a instrction that can jmp to the middle
// 48 05 c3 00 00 00    	add    $0xc3,%rax
// c3 retq
// offset = 2
#define MID_INSTRUCTION \
  asm volatile("mid_instruction: add $0xc3, %%rax;" : : : "rax")

#define POP_STACK \
  asm volatile("pop %%rax" : : : "rax")

// the machine code of a function
//
//  unsigned int func() {
//    return 0;
//  }
//  00:       31 c0                   xor    %eax,%eax
//  02:       c3                      retq
#define FUNC_MACHINE_CODE \
  {0x31, 0xc0, 0xc3}

void FORCE_INLINE assign_fake_machine_code(unsigned char *p) {
  *p++ = 0x31;
  *p++ = 0xc0;
  *p++ = 0xc3;
}

#define JMP_FUNC(pFunc)   \
  asm volatile(           \
    "jmp *%0;"            \
    : : "r" (pFunc)       \
                          )
