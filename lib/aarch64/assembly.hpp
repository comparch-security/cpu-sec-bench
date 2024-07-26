// assembly helper functions
// riscv64

// get the distance between two pointers
#define GET_DISTANCE(dis, pa, pb)            \
  long long pa_tmp = PTR_MASK & pa;          \
  long long pb_tmp = PTR_MASK & pb;          \
  asm volatile(                              \
    "sub %0, %1, %2;"                        \
    : "+r"(dis) : "r" (pa_tmp), "r"(pb_tmp)  )

#define GET_DISTANCE_IMM(dis, pa, pb)        \
  asm volatile(                              \
    "sub %0, %1, %2;"                        \
    : "=r"(dis) : "r" (pa), "r"(pb)          )

// stack related
// mac M1 llvm -O2 will optimize ldr instruction out
// it's best to use READ_STACK_DAT_IMM
#define READ_STACK_DAT(dat, offset)          \
  asm volatile(                              \
    "add  %1, sp, %1;"                       \
    "ldr  %0, [%1];"                         \
    : "=r"(dat) : "r"(offset)                )

#define READ_STACK_DAT_IMM(dat, offset)      \
  asm volatile(                              \
    "ldr %0, [sp, #" #offset "];"            \
    : "=r"(dat)                              )

/* On MAC M1 Clang 12.0.5
 * The `str %1, [%0]' in macro `MOD_STACK_DAT'
 * Has been removed by the compiler even `memory'
 * is added in the clobber list.
 * To cope with this, an explicit C assignement
 * is used instead.
 */

#define GET_RA_ADDR(ra_addr)                 \
  asm volatile(                              \
    "add %0, sp, %0;"                        \
    : "+r"(ra_addr)                          \
  )

#define MOD_STACK_DAT(dat, offset)           \
  asm volatile(                              \
    "add  %0, sp, %0;"                       \
    : "+r"(offset));                         \
  *((void **)offset) = (void *)dat           \

#define SET_MEM(ptr, var)                    \
  asm volatile(                              \
    "str %1, [%0];"                          \
    : : "r" (ptr), "r" (var)                 )

// jump to a pointer
#define JMP_DAT(ptr)                         \
  asm volatile(                              \
    "br   %0;"                               \
    : : "r"(ptr)                             \
                                             )

/* Jump to a data stored in a pointer
 * If load ptr to a register, 
 * br inst will be optimized out in Mac M1
 * Can not use goto statement, because of
 * the label has crossed functions.
*/
#define JMP_DAT_PTR(ptr)                     \
  volatile void* label = *(void**) ptr;      \
  asm volatile(                              \
    "br   %0;"                               \
    : : "r"(label)                           \
                                             )

//pass an integer argument
#define PASS_INT_ARG0_IMM(arg)               \
  asm volatile("mov x0, #" #arg ";")

// test whether the machine support hardware FPU
#if __ARM_FP >= 8
  #define SUPPORT_FP
#endif

// create a fake return stack
#define PUSH_FAKE_RET(ra, fsize)             \
  while(fsize--)                             \
    asm volatile("sub sp,sp,#8;"             \
		 "str %0, [sp];"             \
      : : "r"(ra)                            )

// the machine code for the following
//  d503201f                nop
//  00000000                illegal instruction
#define FUNC_MACHINE_CODE \
  {0x1f, 0x20, 0x03, 0xd5, 0x00, 0x00, 0x00, 0x00}

void FORCE_INLINE assign_fake_machine_code(unsigned char *p) {
  *p++ = 0x1f;
  *p++ = 0x20;
  *p++ = 0x03;
  *p++ = 0xd5;
  *p++ = 0x00;
  *p++ = 0x00;
  *p++ = 0x00;
  *p++ = 0x00;
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

#define GET_SP_LOC(loc) asm volatile("mov %0, sp" : "=r"(loc))
