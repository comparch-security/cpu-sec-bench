// assembly helper functions
// riscv64

// declare a label in assembly
#define DECL_LABEL(label)                    \
  asm volatile(#label ":")

// modify stack
#define MOD_STACK_LABEL(label, offset)       \
  asm volatile(                              \
    "adr  x8," #label ";"                    \
    "str  x8, [sp, %0];"                     \
    : : "i"(offset)                          \
    : "x8"                                   )

#define MOD_STACK_DAT(dat, offset)           \
  asm volatile(                              \
    "str  %1, [sp, %0];"                     \
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
    : "x30"                                  )

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
  PASS_INT_ARG##Idx(arg                      )

#define PASS_INT_ARG0(arg)                   \
  asm volatile("mv a0, %0;" : : "r" (arg) : "a0")
#define PASS_INT_ARG1(arg)                   \
  asm volatile("mv a1, %0;" : : "r" (arg) : "a1")

#ifdef __riscv_float_abi_double
// pass a double argument
#define PASS_DOUBLE_ARG_FROM_INT(Idx, arg)   \
  asm volatile(                              \
    "fmv.d.x fa" #Idx ", %0;"                \
    : : "r" (arg)                            \
    : "fa" #Idx                              )
#endif

// push an address
#define PUSH_LABEL(label)                    \
  asm volatile(                              \
    "la t0, " #label ";"                     \
    "addi sp, sp, -16;"                      \
    "sd t0, 8(sp);"                          \
    : : : "t0"                               )
// create a fake return stack
#define PUSH_FAKE_RET(label)                 \
  asm volatile(                              \
    "la t0, " #label ";"                     \
    "addi sp, sp, -16;"                      \
    "sd t0, 8(sp);"                          \
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

extern void replace_got_func(void **org, void **fake);
