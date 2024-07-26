// assembly helper functions
// x86_64 visualc++
#pragma intrinsic(_AddressOfReturnAddress)
#pragma intrinsic(_ReturnAddress)
#pragma intrinsic(__movsq)

#ifndef X64_MSVC_ASSEMBLY_HPP_INCLUDED
#define X64_MSVC_ASSEMBLY_HPP_INCLUDED

extern "C" void push_fake_ret(void*,  arch_int_t);

union x{void(*func_ptr)(); long long func_num;};
GLOBAR_VAR_PRE x jum_target;
GLOBAR_VAR_PRE CONTEXT sp_loc_context;
FORCE_NOINLINE void func_to_modify_caller_parameter(int target_register);

// get the distance between two pointers
#define GET_DISTANCE(dis, pa, pb)            \
  dis = pa - pb

#define GET_DISTANCE_IMM(dis, pa, pb)        \
  dis = pa - pb

// stack related
#define READ_STACK_DAT(dat, offset) READ_STACK_DAT_IMM(dat, offset)

#define READ_STACK_DAT_IMM(dat, offset)      \
  RtlCaptureContext(&sp_loc_context);        \
  dat = (void*)*(long long*)((long long)sp_loc_context.Rsp + offset);

#define GET_RA_ADDR(ra_addr)                 \
  RtlCaptureContext(&sp_loc_context);        \
  ra_addr += (long long)sp_loc_context.Rsp ; \

#define MOD_STACK_DAT(dat, offset)           \
  RtlCaptureContext(&sp_loc_context);        \
  offset += (long long)sp_loc_context.Rsp ;  \
  void** ptr = (void**)offset;               \
  *ptr = dat

#define SET_MEM(ptr, var)                    \
  __movsq((unsigned long long*)ptr, (const unsigned long long*)(&var),1)

// jump to a pointer
#define JMP_DAT(ptr)                         \
  jum_target.func_num = (long long) ptr;     \
  jum_target.func_ptr()

#define JMP_DAT_PTR(ptr)                     \
  jum_target.func_num = (long long) ptr;     \
  jum_target.func_ptr()

#define GOTO_SITEA {}

//pass an integer argument
//msvc x64 take _fast_call convention as default func-call abi convention
//rcx rdx r8 r9 store 1~4 arguments
#define PASS_INT_ARG0_IMM(arg)               \
  func_to_modify_caller_parameter(arg)

// assume x86_64 always support hardware FPU
#define SUPPORT_FP

// create a fake return stack
#define PUSH_FAKE_RET(ra, fsize)             \
  push_fake_ret(ra,fsize)

// the machine code for the following
//  31 c0                   xor    %eax,%eax
//  31 f6                   xor    %esi,%esi
//  f7 fe                   idiv   %esi
#define FUNC_MACHINE_CODE \
  {0x31, 0xc0, 0x31, 0xf6, 0xf7, 0xfe, 0x00, 0x00}

void FORCE_INLINE assign_fake_machine_code(unsigned char *p) {
  *p++ = 0x31;
  *p++ = 0xc0;
  *p++ = 0x31;
  *p++ = 0xf6;
  *p++ = 0xf7;
  *p++ = 0xfe;
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

#define GET_SP_LOC(loc)                \
  RtlCaptureContext(&sp_loc_context);  \
  loc = sp_loc_context.Rsp

//this macro is used for recorrecting sp-ra offset(only in windows msvc)
#define GET_RAA_SP_OFFSET(offset)                             \
  RtlCaptureContext(&sp_loc_context);                         \
  offset = (arch_int_t)((long long) _AddressOfReturnAddress()-\
           (long long) sp_loc_context.Rsp)
#endif