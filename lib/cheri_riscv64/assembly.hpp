extern "C" void assembly_helper(void* target_address);
extern "C" void assembly_return_site();

  // get the distance between two pointers
  #define GET_DISTANCE(dis, pa, pb)            \
    asm volatile(                              \
      "sub %0, %1, %2;"                        \
      : "+r"(dis) :"r" (pa), "r"(pb))

  #define GET_DISTANCE_IMM(dis, pa, pb)        \
    asm volatile(                              \
      "sub %0, %1, %2;"                        \
      : "+r"(dis) :"r" (pa), "r"(pb))

  // stack related
  #define READ_STACK_DAT(dat, offset)          \
    asm volatile(                              \
      "add %1,sp,%1;"                          \
      "cincoffset ca1,csp,%1;"                 \
      "clc %0,(ca1);"                          \
      : "=C"(dat) :  "r"(offset)              )

  // to avoid error: optional integer offset must be 0
  #define READ_STACK_DAT_IMM(dat, offset)      \
    asm volatile(                              \
      "li x10,"#offset";"                      \
      "cincoffset ca0, csp, x10;"              \
      "clc %0, (ca0);"                         \
      : "=C"(dat) : :"ca0"                     )

  #define GET_SP_BASE(ra_addr)                 \
    asm volatile(                              \
      "add %0, sp, %0;"                        \
      : "+r"(ra_addr)                          \
                                               )

#define GET_RA_ADDR(ra_addr)                 \
  asm volatile(                              \
    "cincoffset %0, cra, %0;"                \
    : "=r"(ra_addr)                          )

  #define MOD_STACK_DAT(dat, offset)           \
    asm volatile(                              \
      "cincoffset ca0, csp, %0;"               \
      "csc %1, 0(ca0);"                        \
      : :"r"(offset), "C"(dat)                 )

  #define SET_MEM(ptr, var)                    \
    asm volatile(                              \
      "sc.cap %1, 0(%0);"                      \
      : : "C" (ptr), "C" (var)                 )

  // jump to a pointer
  #define JMP_DAT(ptr)                         \
    asm volatile(                              \
      "cjalr ca0, %0, 0;"                      \
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

  //cheri-riscv64 force 128 bits address alignment
  #define RA_POS_TEST           \
    RA_POS_TEST_ENTRY(0, 0)     \
    RA_POS_TEST_ENTRY(16, 4)    \
    RA_POS_TEST_ENTRY(32, 8)    \
    RA_POS_TEST_ENTRY(48, 12)   \
    RA_POS_TEST_ENTRY(64, 16)   \
    RA_POS_TEST_ENTRY(80, 20)   \
    RA_POS_TEST_ENTRY(96, 24)
