// assembly helper functions
// x86_64

// load the address of a label to a register
#define LOAD_LABEL(label, v)                 \
  asm volatile(                              \
    "lea " #label "(%%rip), %0;"             \
    : "+r"(v) :                              )

// declare a label in assembly
#define DECL_LABEL(label)                    \
  asm volatile(#label ":")

// get the distance between two pointers
#define GET_DISTANCE(dis, pa, pb)            \
  asm volatile(                              \
    "movq %1, %0;"                           \
    "subq %2, %0;"                           \
    : "+r"(dis) : "r" (pa), "r"(pb)          ) 

// modify stack
#define MOD_STACK_LABEL(label, offset)       \
  asm volatile(                              \
    "lea " #label "(%%rip), %%rax;"          \
    "movl %0, %%ecx;"                        \
    "movslq %%ecx, %%rcx;"                   \
    "addq %%rsp, %%rcx;"                     \
    "movq %%rax, (%%rcx);"                   \
    : : "r"(offset)                          \
    : "rax", "rcx"                           )

#define MOD_STACK_DAT(dat, offset)           \
  asm volatile(                              \
    "movl %1, %%ecx;"                        \
    "movslq %%ecx, %%rcx;"                   \
    "addq %%rsp, %%rcx;"                     \
    "movq %0, (%%rcx);"                      \
    : : "r"(dat), "r"(offset)                \
    : "rcx"                                  )

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

#define CALL_DAT_INT(ptr, arg0)              \
  asm volatile(                              \
    "mov  %1, %%rdi;"                        \
    "call  *%0;"                             \
    : : "r"(ptr), "r"(arg0)                  \
    : "rdi"                                  )

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
  asm volatile("mov %0, %%rdi;" : : "r" (arg))
#define PASS_INT_ARG1(arg)                   \
  asm volatile("mov %0, %%rsi;" : : "r" (arg))

// assume x86_64 always support hardware FPU
#define SUPPORT_FP

// pass a double argument
#define PASS_DOUBLE_ARG_FROM_INT(Idx, arg)   \
  asm volatile(                              \
    "movq %0, %%rax;"                        \
    "movq %%rax, %%xmm" #Idx ";"             \
    : : "r" (arg)                            \
    : "rax"                                  )

// create a fake return stack
#define PUSH_FAKE_RET(label)                 \
  asm volatile(                              \
    "lea " #label "(%%rip), %%rax;"          \
    "push %%rax;"                            \
    "subq %0, %%rsp;"                        \
    : : "i"(8)                               \
    : "rax"                                  )

// a instrction that can jmp to the middle
// 48 05 c3 00 00 00    	add    $0xc3,%rax
// c3 retq
// offset = 2
#define MID_INSTRUCTION \
  asm volatile("mid_instruction: add $0xc3, %%rax;" : : : "rax")

// the machine code for the following
//  31 c0                   xor    %eax,%eax
//  31 f6                   xor    %esi,%esi
//  f7 fe                   idiv   %esi
#define FUNC_MACHINE_CODE \
  {0x31, 0xc0, 0x31, 0xf6, 0xf7, 0xfe, 0x00, 0x00}

void FORCE_INLINE assign_fake_machine_code(unsigned char *p) {
  *p++ = 0x31;
  *p++ = 0xc0;
  *p++ = 0x31;
  *p++ = 0xf6;
  *p++ = 0xf7;
  *p++ = 0xfe;
}

extern void get_got_func(void **gotp, int stack_offset);
extern void replace_got_func(void **fake, void *got);
