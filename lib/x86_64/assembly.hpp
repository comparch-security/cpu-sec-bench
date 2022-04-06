// assembly helper functions
// x86_64

// special macros for acc-read-func
#define READ_FUNC_CODE 0x0000f701
#define READ_FUNC_MASK 0x0000ffff

// get the distance between two pointers
#define GET_DISTANCE(dis, pa, pb)            \
  asm volatile(                              \
    "movq %1, %0;"                           \
    "subq %2, %0;"                           \
    : "+r"(dis) : "r" (pa), "r"(pb)          ) 

// stack related
#define READ_STACK_DAT(dat, offset)          \
  asm volatile(                              \
    "movq %1, %%rcx;"                        \
    "addq %%rsp, %%rcx;"                     \
    "movq (%%rcx), %0;"                      \
    : "+r"(dat) : "r"(offset)                \
    : "rcx"                                  )

#define READ_STACK_DAT_IMM(dat, offset)      \
  asm volatile(                              \
    "movq " #offset "(%%rsp), %0;"           \
    : "=r"(dat)                              )

#define MOD_STACK_DAT(dat, offset)           \
  asm volatile(                              \
    "movq %1, %%rcx;"                        \
    "addq %%rsp, %%rcx;"                     \
    "movq %0, (%%rcx);"                      \
    : : "r"(dat), "r"(offset)                \
    : "rcx"                                  )

#define SET_MEM(ptr, var)                    \
  asm volatile(                              \
    "movq %1, (%0);"                         \
    : : "r" (ptr), "r" (var)                 )

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

// jump to a pointer
#define JMP_DAT(ptr)                         \
  asm volatile(                              \
    "jmp *%0;"                               \
    : : "r" (ptr)                            \
                                             )

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
#define PUSH_FAKE_RET(ra, fsize)             \
  while(fsize--)                             \
    asm volatile("push %0;"                  \
      : : "r"(ra)                            )

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
