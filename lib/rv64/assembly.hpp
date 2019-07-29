// assembly helper functions
// RV64

// declare a label in assembly
#define DECL_LABEL(label) \
  asm volatile(#label ":")

//call to a label
#define CALL_LABEL(label)                    \
  asm volatile(                              \
    "call " #label ";"                       \
                                             )

//call to a label with offset
#define CALL_LABEL_OFFSET(label,offset)      \
         asm volatile(                       \
	"la a0," #label ";"                  \
	"addi a0,a0,%0;"	             \
	"jalr ra,a0;"                        \
	::"i"(offset):                       )
    
//call to a pointer
#define CALL_DAT(ptr)                        \
  asm volatile(                              \
    "jalr ra,0(%0);"                         \
      ::"r"(ptr): "ra"                       )

//jump to a label
#define JUMP_LABEL(label)                    \
  asm volatile(                              \
    "j " #label ";"                          )

//jump to a label with offset
#define JUMP_LABEL_OFFSET(label,offset)      \
         asm volatile(                       \
	"la a0," #label ";"                  \
	"addi a0,a0,%0;"	             \
	"jalr x0,a0;"                        \
	::"i"(offset):                       )

//jump to a pointer
#define JUMP_DAT(ptr)                        \
  asm volatile(                              \
    "jalr x0,0(%0);"                         \
      ::"r"(ptr):                            )

//exchange memory value 
#define XCHG_MEM(ptrL, ptrR)                 \
  asm volatile(                              \
    "ld s2,0(%1);"                           \
    "sd s2,0(%0);"                           \
    : : "r" (ptrL), "r" (ptrR)               \
    : "s2"                                   \
                                             )
//pass an integer arugument
#define PASS_INT_ARG(arg)                    \
  asm volatile(                              \
    "mv a0,%0;" : : "r" (arg): "a0"          )

//pass an double arugument
#define PASS_DOUBLE_ARG(arg)                 \
  asm volatile(                              \
    "fmv.d fa0,%0;" : : "f" (arg): "fa0"     )


//modify return address to a pointer
#define MOD_RET_DAT(dat)                     \
  asm volatile(                              \
    "mv ra,%0;"                              \
    ::"r"(dat):                              )

//modify return address to a label
#define MOD_RET_LABEL(label)                 \
  asm volatile(                              \
    "la ra," #label ";"                      \
                                             )

//modify return address to a label with offset
#define MOD_RET_LABEL_(label,offset)         \
  asm volatile(                              \
    "la a0," #label ";"                      \
    "addi a0,a0,%0;"                         \
    "sd a0,8(sp);"                           \
    ::"i"(offset): "ra"                      \
                                             )

//modify return address to a label while the return address has been defined once
#define MOD_RET_LABEL_DEFINED(label)         \
  asm volatile(                              \
    "la s1," #label ";"                      \
    "sd s1, 8(sp);"                          \
                                             )

//an instruction the can jump to the middle
#define MID_INSTRUCTION                      \
  asm volatile(                              \
    "mid_instruction:lui a0,0x45050;"        )

//pass a0's value to a variable
#define PASS_VALUE(var)                      \
  asm volatile(                              \
    "mv %0,a0;"                              \
    :"=r"(var)::                             )
