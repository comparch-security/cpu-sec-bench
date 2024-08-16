// assembly helper functions
// x86_64 gcc

extern "C" void assembly_helper(void* target_address);
extern "C" void assembly_return_site();

// get the distance between two pointers
#define GET_DISTANCE(dis, pa, pb)            \
  asm volatile(                              \
    "movq %1, %0;"                           \
    "subq %2, %0;"                           \
    : "+r"(dis) : "r" (pa), "r"(pb)          ) 

#define GET_DISTANCE_IMM(dis, pa, pb)        \
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

#define GET_SP_BASE(ra_addr)                 \
  asm volatile(                              \
    "movq %0, %%rcx;"                        \
    "addq %%rsp, %%rcx;"                     \
    "movq %%rcx, %0;"                        \
    : "+r"(ra_addr)                          \
                                             )

#define GET_RA_ADDR(ra_addr)                 \
  asm volatile(                              \
    "movq 8(%%rbp), %0;"                     \
    : "=r"(ra_addr)                          )

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

// jump to a pointer
#define JMP_DAT(ptr)                         \
  asm volatile(                              \
    "jmp *%0;"                               \
    : : "r" (ptr)                            \
                                             )

// jump to a data stored in a pointer
#define JMP_DAT_PTR(ptr)                      \
  asm volatile(                              \
    "mov (%0), %%rcx;"                       \
    "jmp *%%rcx;"                             \
    : : "c" (ptr)                            \
                                             )

//pass an integer argument
#define PASS_INT_ARG0_IMM(arg)               \
  asm volatile("mov $" #arg ", %rdi;")

// assume x86_64 always support hardware FPU
#define SUPPORT_FP

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

#define RA_POS_TEST        \
  RA_POS_TEST_ENTRY(0, 0)  \
  RA_POS_TEST_ENTRY(4, 1)  \
  RA_POS_TEST_ENTRY(8, 2)  \
  RA_POS_TEST_ENTRY(12, 3)  \
  RA_POS_TEST_ENTRY(16, 4)  \
  RA_POS_TEST_ENTRY(20, 5)  \
  RA_POS_TEST_ENTRY(24, 6)  \
  RA_POS_TEST_ENTRY(28, 7)  \
  RA_POS_TEST_ENTRY(32, 8)  \
  RA_POS_TEST_ENTRY(36, 9)  \
  RA_POS_TEST_ENTRY(40, 10)  \
  RA_POS_TEST_ENTRY(44, 11)  \
  RA_POS_TEST_ENTRY(48, 12)  \
  RA_POS_TEST_ENTRY(52, 13)  \
  RA_POS_TEST_ENTRY(56, 14)  \
  RA_POS_TEST_ENTRY(60, 15)

#define GET_SP_LOC(loc) asm volatile("mov %%rsp, %0;" : "=r"(loc))
