// assembly helper functions
// riscv64

// special macros for the read-func test
#define READ_FUNC(var, num) var += num
#define READ_FUNC_CODE 0x0b000028
#define READ_FUNC_MASK 0xffffffff

// declare a label in assembly
#define DECL_LABEL(label)                    \
  asm volatile(#label ":")

// get the distance between two pointers
#define GET_DISTANCE(dis, pa, pb)            \
  asm volatile(                              \
    "sub %0, %1, %2;"                        \
    : "+r"(dis) : "r" (pa), "r"(pb)          ) 

// modify stack
#define MOD_STACK_LABEL(label, offset)       \
  asm volatile(                              \
    "adr  x8," #label ";"                    \
    "add  %0, sp, %0;"                       \
    "str  x8, [%0];"                         \
    : : "r"(offset)                          \
    : "x8"                                   )

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

#define MOD_STACK_DAT(dat, offset)           \
  asm volatile(                              \
    "add  %0, sp, %0;"                       \
    : "+r"(offset));                         \
  *((void **)offset) = (void *)dat           \

// exchange memory value
#define XCHG_MEM(ptrL, ptrR)                 \
  asm volatile(                              \
    "ldr  x8, [%1];"                         \
    "ldr  x9, [%0];"                         \
    "str  x9, [%1];"                         \
    "str  x8, [%0];"                         \
    : : "r" (ptrL), "r" (ptrR)               \
    : "x8", "x9"                             )

#define SET_MEM(ptr, var)                    \
  asm volatile(                              \
    "str %1, [%0];"                          \
    : : "r" (ptr), "r" (var)                 )

// call to a pointer
#define CALL_DAT(ptr)                        \
  asm volatile(                              \
    "blr  %0;"                               \
    : : "r"(ptr)                             )

#define CALL_DAT_INT(ptr, arg0)              \
  asm volatile(                              \
    "mov  x0, %1;"                           \
    "blr  %0;"                               \
    : : "r"(ptr), "r"(arg0)                  \
    : "x0", "x30"                            )

//call to a label
#define CALL_LABEL(label, offset)            \
  asm volatile(                              \
   "adr   x8," #label ";"                    \
   "add   x8, x8, %0;"                       \
   "blr   x8;"                               \
   : : "i"(offset) : "x8"                    )

// jump to a pointer
#define JMP_DAT(ptr)                         \
  asm volatile(                              \
    "br   %0;"                               \
    : : "r"(ptr)                             \
                                             )

// jump to a label with offset
#define JMP_LABEL(label, offset)             \
  asm volatile(                              \
    "adr   x8," #label ";"                   \
    "add   x8, x8, %0;"                      \
    "br    x8;"                              \
    : : "i"(offset) : "x8"                   )

//pass an integer argument
#define PASS_INT_ARG(Idx, arg)               \
  asm volatile(                              \
    "mov x" #Idx ", %0;"                     \
    : : "r" (arg)                            )

// test whether the machine support hardware FPU
#if __ARM_FP >= 8
  #define SUPPORT_FP
#endif

// pass a double argument
#define PASS_DOUBLE_ARG_FROM_INT(Idx, arg)   \
  asm volatile(                              \
    "fmov d" #Idx ", %0;"                    \
    : : "r" (arg)                            )

// create a fake return stack
#define PUSH_FAKE_RET(ra, fsize)             \
  while(fsize--)                             \
    asm volatile("str %0, [sp, #-8]!;"       \
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
