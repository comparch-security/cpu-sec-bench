// assembly helper functions
// riscv64

#if EXTEND_ISA == CHERI

  // get the distance between two pointers
  #define GET_DISTANCE(dis, pa, pb)            \
    asm volatile(                              \
      "sub x0, %1, %2;"                       \
      : "+r"(dis) :"r" (pa), "r"(pb))

  // stack related
  #define READ_STACK_DAT(dat, offset)          \
    asm volatile(                              \
      "cgetaddr x0,%1;"                        \
      "add x0,sp,x0;"                          \
      "csetaddr ca0,csp,x0;"                   \
      "lc.cap %0,(ca0);"                       \
      : "=C"(dat) : "C"(offset)                )

  // to avoid error: optional integer offset must be 0
  #define READ_STACK_DAT_IMM(dat, offset)      \
    asm volatile(                              \
      "li x0," #offset ";"                     \
      "add x0, x0,sp;"                         \
      "csetaddr ca0,csp,x0;"                   \
      "lc.cap %0, (ca0);"                      \
      : "=C"(dat)                              )

  #define MOD_STACK_DAT(dat, offset)           \
    asm volatile(                              \
      "cincoffset ca0, csp, %0;"               \
      "csc %1, 0(ca0);"                        \
      : :"r"(offset), "C"(dat)                )

  #define SET_MEM(ptr, var)                    \
    asm volatile(                              \
      "sc.cap %1, 0(%0);"                      \
      : : "C" (ptr), "C" (var)                 )

  // jump to a pointer
  #define JMP_DAT(ptr)                         \
    asm volatile(                              \
      "cjalr ca0, %0, 0;"                        \
      : : "C"(ptr)                             \
                                              )

  // jump to a dat stored in a pointer
  #define JMP_DAT_PTR(ptr)                      \
    asm volatile(                              \
      "clc ca1,(%0);"                              \
      "cjalr ca0, ca1, 0;"                        \
      : : "C"(ptr)                             \
      : "ca1"                                   \
                                              )

  //pass an integer argument
  #define PASS_INT_ARG0_IMM(arg)               \
    asm volatile("lui a0, " #arg ";")

  // test whether the machine support hardware FPU
  #ifdef __riscv_float_abi_double
    #define SUPPORT_FP
  #endif

  // create a fake return stack
  #define PUSH_FAKE_RET(ra, fsize)             \
    while(fsize--)                             \
      asm volatile(                            \
        "cincoffset csp, csp, -8;"             \
        "csc   %0, (csp);"                     \
        : : "C"(ra)                            )

  // the machine code for the following
  // 0001                    nop
  // 0001                    nop
  // 0000                    illegal instruction
  // 0000                    illegal instruction
  #define FUNC_MACHINE_CODE \
    {0x01, 0x00, 0x01, 0x00, 0x00, 0x00, 0x00, 0x00}

#else

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

  // jump to a pointer
  #define JMP_DAT(ptr)                         \
    asm volatile(                              \
      "jalr x0, %0, 0;"                        \
      : : "r"(ptr)                             \
                                              )

  // jump to a dat stored in a pointer
  #define JMP_DAT_PTR(ptr)                      \
    asm volatile(                              \
      "ld t0,(%0);"                              \
      "jalr x0, t0, 0;"                        \
      : : "r"(ptr)                             \
      : "t0"                                   \
                                              )

  //pass an integer argument
  #define PASS_INT_ARG0_IMM(arg)               \
    asm volatile("lui a0, " #arg ";")

  // test whether the machine support hardware FPU
  #ifdef __riscv_float_abi_double
    #define SUPPORT_FP
  #endif

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

#endif

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

