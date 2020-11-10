// assembly helper functions
// x86_64

// declare a label in assembly
#define DECL_LABEL(label)                    \
  asm volatile(#label ":")

// modify stack
#define MOD_STACK_LABEL(label, offset)       \
  asm volatile(                              \
    "lea " #label "(%%rip), %%rax;"          \
    "addq %%rsp, %0;"                        \
    "movq %%rax, (%0);"                      \
    : : "r"(offset)                          \
    : "rax"                                  )

#define MOD_STACK_DAT(dat, offset)           \
  asm volatile(                              \
    "addq %%rsp, %0;"                        \
    "movq %1, (%0);"                         \
    : : "r"(offset), "r"(dat)                )

// detect the stack
extern int dummy_leaf_rv;
extern int dummy_leaf_func(int);
#define ENFORCE_NON_LEAF_FUNC dummy_leaf_rv = dummy_leaf_func(dummy_leaf_rv);

// modify return address to a label
#define MOD_RET_LABEL(label)                 \
  MOD_STACK_LABEL(label, 8)
#define MOD_RET_DAT(dat)                     \
  MOD_STACK_DAT(dat, 8)


// exchange memory value
#define XCHG_MEM(ptrL, ptrR)                 \
  asm volatile(                              \
    "movq (%0), %%rax;"                      \
    "xchg %%rax, (%1);"                      \
    "movq %%rax, (%0);"                      \
    : : "r" (ptrL), "r" (ptrR)               \
    : "rax"                                  )

// call to a pointer
#define CALL_DAT(ptr)                        \
  asm volatile(                              \
    "call *%0;"                              \
    : : "r" (ptr)                            )

//call to a label
#define CALL_LABEL(label, offset)            \
  asm volatile(                              \
    "lea " #label "(%%rip), %%rax;"          \
    "add %0, %%rax;"                         \
    "call *%%rax;"                           \
    : : "i"(offset) : "rax"                  )

// jump to a pointer
#define JMP_DAT(ptr)                         \
  asm volatile(                              \
    "jmp *%0;"                               \
    : : "r" (ptr)                            \
                                             )

// jump to a label with offset
#define JMP_LABEL(label, offset)             \
  asm volatile(                              \
    "lea " #label "(%%rip), %%rax;"          \
    "add %0, %%rax;"                         \
    "jmp *%%rax;"                            \
    : : "i"(offset) : "rax"                  )

//pass an integer argument
#define PASS_INT_ARG(Idx, arg)               \
  PASS_INT_ARG##Idx(arg                      )

#define PASS_INT_ARG0(arg)                   \
  asm volatile("mov %0, %%rdi;" : : "r" (arg) : "rdi")
#define PASS_INT_ARG1(arg)                   \
  asm volatile("mov %0, %%rsi;" : : "r" (arg) : "rsi")

// pass a double argument
#define PASS_DOUBLE_ARG_FROM_INT(Idx, arg)   \
  asm volatile(                              \
    "movq %0, %%rax;"                        \
    "movq %%rax, %%xmm" #Idx ";"             \
    : : "r" (arg)                            \
    : "rax", "xmm" #Idx                      )

// push an address
#define PUSH_LABEL(label)                    \
  asm volatile(                              \
    "lea " #label "(%%rip), %%rax;"          \
    "push %%rax;"                            \
    : : : "rax"                              )
// create a fake return stack
#define PUSH_FAKE_RET(label)                 \
  asm volatile(                              \
    "lea " #label "(%%rip), %%rax;"          \
    "push %%rax;"                            \
    "subq %0, %%rsp;"                        \
    : : "i"(8)                               \
    : "rax"                                  )

// return
#define RET \
  asm volatile("ret")

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

extern void replace_got_func(void **org, void **fake);
