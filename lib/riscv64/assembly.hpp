// assembly helper functions
// riscv64

// load the address of a label to a register
#define LOAD_LABEL(label, v)                 \
  asm volatile(                              \
    "la %0," #label ";"                      \
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
    "la  t0," #label ";"                     \
    "sd  t0, %0(sp);"                        \
    : : "i"(offset)                          \
    : "t0"                                   )

#define MOD_STACK_DAT(dat, offset)           \
  asm volatile(                              \
    "sd  %1, %0(sp);"                        \
    : : "i"(offset), "r"(dat)                )

// detect the stack
extern int dummy_leaf_rv;
extern int dummy_leaf_func(int);
#define ENFORCE_NON_LEAF_FUNC_VAR(VAR) dummy_leaf_rv = dummy_leaf_func(VAR);
#define ENFORCE_NON_LEAF_FUNC dummy_leaf_rv = dummy_leaf_func(dummy_leaf_rv);

// modify return address to a label
#define MOD_RET_LABEL(label)                 \
  MOD_STACK_LABEL(label, 8)
#define MOD_RET_DAT(dat)                     \
  MOD_STACK_DAT(dat, 8)


// exchange memory value
#define XCHG_MEM(ptrL, ptrR)                 \
  asm volatile(                              \
    "ld t0, 0(%1);"                          \
    "ld t1, 0(%0);"                          \
    "sd t1, 0(%1);"                          \
    "sd t0, 0(%0);"                          \
    : : "r" (ptrL), "r" (ptrR)               \
    : "t0", "t1"                             )

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

//call to a label
#define CALL_LABEL(label, offset)            \
  asm volatile(                              \
   "la   t0," #label ";"                     \
   "jalr ra, t0, %0;"                        \
   : : "i"(offset) : "t0"                    )

// jump to a pointer
#define JMP_DAT(ptr)                         \
  asm volatile(                              \
    "jalr x0, %0, 0;"                        \
    : : "r"(ptr)                             \
                                             )

// jump to a label with offset
#define JMP_LABEL(label, offset)             \
  asm volatile(                              \
    "la   t0," #label ";"                    \
    "jalr x0, t0, %0;"                       \
    : : "i"(offset) : "t0"                   )

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

// push an address
#define PUSH_LABEL(label)                    \
  asm volatile(                              \
    "la   t0, " #label ";"                   \
    "addi sp, sp, -16;"                      \
    "sd   t0, 8(sp);"                        \
    : : : "t0"                               )
// create a fake return stack
#define PUSH_FAKE_RET(label)                 \
  asm volatile(                              \
    "la   t0, " #label ";"                   \
    "addi sp, sp, -16;"                      \
    "sd   t0, 8(sp);"                        \
    : : : "t0"                               )

// return
#define RET \
  asm volatile("addi sp, sp, 16; ret")

// the machine code for the following
// 357d                    addiw   a0,a0,-1
// 60a2                    ld      ra,8(sp)
// 0141                    addi    sp,sp,16
// 8082                    ret
#define FUNC_MACHINE_CODE \
  {0x7d, 0x35, 0xa2, 0x60, 0x41, 0x01, 0x82, 0x80}

void FORCE_INLINE assign_fake_machine_code(unsigned char *p) {
  *p++ = 0x7d;
  *p++ = 0x35;
  *p++ = 0xa2;
  *p++ = 0x60;
  *p++ = 0x41;
  *p++ = 0x01;
  *p++ = 0x82;
  *p++ = 0x80;
}

// the machine code for the following
// 4501                    li      a0,0
// 60a2                    ld      ra,8(sp)
// 0141                    addi    sp,sp,16
// 8082                    ret
#define FUNC_MACHINE_CODE_CALL \
  {0x01, 0x45, 0xa2, 0x60, 0x41, 0x01, 0x82, 0x80}

void FORCE_INLINE assign_fake_machine_code_call(unsigned char *p) {
  *p++ = 0x01;
  *p++ = 0x45;
  *p++ = 0xa2;
  *p++ = 0x60;
  *p++ = 0x41;
  *p++ = 0x01;
  *p++ = 0x82;
  *p++ = 0x80;
}

extern void replace_got_func(void **fake);
