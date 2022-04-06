// assembly helper functions
// riscv64

// special macros for acc-read-func
#define READ_FUNC_CODE 0x00009d2d
#define READ_FUNC_MASK 0x0000ffff

// get the distance between two pointers
#define GET_DISTANCE(dis, pa, pb)            \
  asm volatile(                              \
    "sub %0, %1, %2;"                        \
    : "+r"(dis) : "r" (pa), "r"(pb)          ) 

// stack related
#define READ_STACK_DAT(dat, offset)          \
  asm volatile(                              \
    "add  %1, sp, %1;"                       \
    "ld   %0, (%1);"                         \
    : "=r"(dat) : "r"(offset)                )

#define READ_STACK_DAT_IMM(dat, offset)      \
  asm volatile(                              \
    "ld %0, " #offset "(sp);"                \
    : "=r"(dat)                              )

#define MOD_STACK_DAT(dat, offset)           \
  asm volatile(                              \
    "add %0, %0, sp;"                        \
    "sd  %1, (%0);"                          \
    : : "r"(offset), "r"(dat)                )

#define SET_MEM(ptr, var)                    \
  asm volatile(                              \
    "sd %1, 0(%0);"                          \
    : : "r" (ptr), "r" (var)                 )

// call to a pointer
#define CALL_DAT(ptr)                        \
  asm volatile(                              \
    "jalr ra, %0, 0;"                        \
    : : "r"(ptr)                             \
    : "ra"                                   )

#define CALL_DAT_INT(ptr, arg0)              \
  asm volatile(                              \
    "mv    a0, %1;"                          \
    "jalr  ra, %0, 0;"                       \
    : : "r"(ptr), "r"(arg0)                  \
    : "a0", "ra"                             )

// jump to a pointer
#define JMP_DAT(ptr)                         \
  asm volatile(                              \
    "jalr x0, %0, 0;"                        \
    : : "r"(ptr)                             \
                                             )

//pass an integer argument
#define PASS_INT_ARG(Idx, arg)               \
  asm volatile(                              \
    "mv a" #Idx ", %0;"                      \
    : : "r" (arg)                            \
    : "a" #Idx                               )

// test whether the machine support hardware FPU
#ifdef __riscv_float_abi_double
  #define SUPPORT_FP
#endif

// pass a double argument
#define PASS_DOUBLE_ARG_FROM_INT(Idx, arg)   \
  asm volatile(                              \
    "fmv.d.x fa" #Idx ", %0;"                \
    : : "r" (arg)                            \
    : "fa" #Idx                              )

// create a fake return stack
#define PUSH_FAKE_RET(ra, fsize)             \
  while(fsize--)                             \
    asm volatile(                            \
      "addi sp, sp, -8;"                     \
      "sd   %0, (sp);"                       \
      : : "r"(ra)                            )

// the machine code for the following
// 0001                    nop
// 0001                    nop
// 0000                    illegal instruction
// 0000                    illegal instruction
#define FUNC_MACHINE_CODE \
  {0x01, 0x00, 0x01, 0x00, 0x00, 0x00, 0x00, 0x00}

void FORCE_INLINE assign_fake_machine_code(unsigned char *p) {
  *p++ = 0x01;
  *p++ = 0x00;
  *p++ = 0x01;
  *p++ = 0x00;
  *p++ = 0x00;
  *p++ = 0x00;
  *p++ = 0x00;
  *p++ = 0x00;
}

