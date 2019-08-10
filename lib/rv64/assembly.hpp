// assembly helper functions
// RV64

// declare a label in assembly
#define DECL_LABEL(label)                    \
  asm volatile(#label ":")

//call to a label
#define CALL_LABEL(label,offset)             \
  asm volatile(                              \
   "la t0," #label ";"                       \
   "addi t0,t0,%0;"                          \
   "jalr ra,t0;"                             \
   ::"i"(offset):                            )
    
//call to a pointer
#define CALL_DAT(ptr)                        \
  asm volatile(                              \
    "jalr ra,0(%0);"                         \
      ::"r"(ptr): "ra"                       )

//jump to a label with offset
#define JMP_LABEL(label,offset)              \
  asm volatile(                              \
    "la t0," #label ";"                      \
    "addi t0,t0,%0;"                         \
    "jalr x0,t0;"                            \
    ::"i"(offset):                           )

//jump to a pointer
#define JMP_DAT(ptr)                         \
  asm volatile(                              \
    "jalr x0,0(%0);"                         \
      ::"r"(ptr):                            )

//exchange memory value 
#define XCHG_MEM(ptrL, ptrR)                 \
  asm volatile(                              \
    "ld t0,0(%1);"                           \
    "ld t1,0(%0);"                           \
    "sd t1,0(%1);"                           \
    "sd t0,0(%0);"                           \
    : : "r" (ptrL), "r" (ptrR) :             )

//pass an integer arugument
#define PASS_INT_ARG(Idx, arg)               \
  PASS_INT_ARG##Idx(arg                      )

#define PASS_INT_ARG0(arg)                   \
  asm volatile(                              \
    "mv a0,%0;" : : "r" (arg): "a0"          )
#define PASS_INT_ARG1(arg)                   \
  asm volatile(                              \
    "mv a1,%0;" : : "r" (arg): "a1"          )

//pass an double arugument
#define PASS_DOUBLE_ARG(Idx, arg)            \
  PASS_DOUBLE_ARG##Idx(arg                   )

#define PASS_DOUBLE_ARG0(arg)                \
  asm volatile(                              \
    "fmv.d fa0,%0;" : : "f" (arg): "fa0"     )
#define PASS_DOUBLE_ARG1(arg)                \
  asm volatile(                              \
    "fmv.d fa1,%0;" : : "f" (arg): "fa1"     )

//modify return address to a pointer
#define MOD_RET_DAT(dat)                     \
  asm volatile(                              \
    "sd %0,-8(s0)"                           \
    ::"r"(dat):                              )

//modify return address to a label with offset
#define MOD_RET_LABEL(label,offset)          \
  asm volatile(                              \
    "la t0," #label ";"                      \
    "addi t0,t0,%0;"                         \
    "sd t0,8(sp);"                           \
    ::"i"(offset):                           )

//an instruction the can jump to the middle
#define MID_INSTRUCTION                      \
  asm volatile(                              \
    "mid_instruction:lui a0,0x45050;"        )

//pass a0's value to a variable
#define PASS_VALUE(var)                      \
  asm volatile(                              \
    "mv %0,a0;"                              \
    :"=r"(var)::                             )

#define FUNC_MACHINE_CODE                    \
  {0x80, 0x82}

void FORCE_INLINE assign_fake_machine_code(unsigned char *p) {
  *p++ = 0x80;
  *p++ = 0x82;
}
