// assembly helper functions
// riscv64

// load the address of a label to a register
#define LOAD_LABEL(label, v)                 \
  asm volatile(                              \
    "adr  %0," #label ";"                    \
    : "+r"(v)                                )

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

#define MOD_STACK_DAT(dat, offset)           \
  asm volatile(                              \
    "add  %0, sp, %0;"                       \
    "str  %1, [%0];"                         \
    : : "r"(offset), "r"(dat)                )

// exchange memory value
#define XCHG_MEM(ptrL, ptrR)                 \
  asm volatile(                              \
    "ldr  x8, [%1];"                         \
    "ldr  x9, [%0];"                         \
    "str  x9, [%1];"                         \
    "str  x8, [%0];"                         \
    : : "r" (ptrL), "r" (ptrR)               \
    : "x8", "x9"                             )

// call to a pointer
#define CALL_DAT(ptr)                        \
  asm volatile(                              \
    "blr  %0;"                               \
    : : "r"(ptr)                             \
    : "x30"				     )

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
    : : "r" (arg)                            \
    : "x" #Idx                               )

// test whether the machine support hardware FPU
#if __ARM_FP >= 8
  #define SUPPORT_FP
#endif

// pass a double argument
#define PASS_DOUBLE_ARG_FROM_INT(Idx, arg)   \
  asm volatile(                              \
    "fmov d" #Idx ", %0;"                    \
    : : "r" (arg)                            \
    : "d" #Idx                               )

// create a fake return stack
#define PUSH_FAKE_RET(label)                 \
  asm volatile(                              \
    "adr  x8, " #label ";"                   \
    "stp  x29, x8, [sp, -32]!;"              \
    "mov  x29, sp;"                          \
    : : : "x8"                               )

// return
#define RET \
  asm volatile("ldp x29, x30, [sp], 32; ret;")

// the machine code for the following
// d1000400                sub     x0, x0, -1
// a8c27bfd                ldp     x29, x30, [sp], 32
// d65f03c0                ret
#define FUNC_MACHINE_CODE \
  {0x00, 0x04, 0x00, 0xd1, 0xfd, 0x7b, 0xc2, 0xa8, 0xc0, 0x03, 0x5f, 0xd6}

void FORCE_INLINE assign_fake_machine_code(unsigned char *p) {
  *p++ = 0x00;
  *p++ = 0x04;
  *p++ = 0x00;
  *p++ = 0xd1;
  *p++ = 0xfd;
  *p++ = 0x7b;
  *p++ = 0xc2;
  *p++ = 0xa8;
  *p++ = 0xc0;
  *p++ = 0x03;
  *p++ = 0x5f;
  *p++ = 0xd6;
}

// the machine code for the following
//  d2800000                mov  x0, 0
//  9e670000                fmov d0, x0
//  1e601800                fdiv d0, d0, d0
#define FUNC_MACHINE_CODE_RETURN \
  {0x00, 0x00, 0x80, 0xd2, 0x00, 0x00, 0x67, 0x9e, 0x00, 0x18, 0x60, 0x1e}

void FORCE_INLINE assign_fake_machine_code_return(unsigned char *p) {
  *p++ = 0x00;
  *p++ = 0x00;
  *p++ = 0x80;
  *p++ = 0xd2;
  *p++ = 0x00;
  *p++ = 0x00;
  *p++ = 0x67;
  *p++ = 0x9e;
  *p++ = 0x00;
  *p++ = 0x18;
  *p++ = 0x60;
  *p++ = 0x1e;
}

// the machine code for the following
// d2800000                mov     x0, 0
// a8c27bfd                ldp     x29, x30, [sp], 32
// d65f03c0                ret
#define FUNC_MACHINE_CODE_CALL \
  {0x00, 0x00, 0x80, 0xd2, 0xfd, 0x7b, 0xc2, 0xa8, 0xc0, 0x03, 0x5f, 0xd6}

void FORCE_INLINE assign_fake_machine_code_call(unsigned char *p) {
  *p++ = 0x00;
  *p++ = 0x00;
  *p++ = 0x80;
  *p++ = 0xd2;
  *p++ = 0xfd;
  *p++ = 0x7b;
  *p++ = 0xc2;
  *p++ = 0xa8;
  *p++ = 0xc0;
  *p++ = 0x03;
  *p++ = 0x5f;
  *p++ = 0xd6;
}

extern void get_got_func(void **gotp, int stack_offset);
extern void replace_got_func(void **fake, void *got);
