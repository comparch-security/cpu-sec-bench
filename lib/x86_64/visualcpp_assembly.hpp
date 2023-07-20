// assembly helper functions
// x86_64 visualc++
#pragma intrinsic(_AddressOfReturnAddress)
#pragma intrinsic(_ReturnAddress)
#pragma intrinsic(__movsq)

#ifndef X64_MSVC_ASSEMBLY_HPP_INCLUDED
#define X64_MSVC_ASSEMBLY_HPP_INCLUDED

union x{void(*func_ptr)(); long long func_num;};
GLOBAR_VAR_PRE x jum_target;
GLOBAR_VAR_PRE CONTEXT sp_loc_context;

// get the distance between two pointers
#define GET_DISTANCE(dis, pa, pb)            \
  dis = pa - pb

// stack related
#define READ_STACK_DAT(dat, offset)          \
  dat = *(long long*)((long long)_AddressOfReturnAddress() + offset)

#define READ_STACK_DAT_IMM(dat, offset)      \
  RtlCaptureContext(&sp_loc_context);        \
  dat = (void*)*(long long*)((long long)sp_loc_context.Rsp + offset);

#define MOD_STACK_DAT(dat, offset)                                        \
  void** ptr = (void**)((long long)_AddressOfReturnAddress() + offset);   \
  *ptr = dat

#define SET_MEM(ptr, var)                    \
  __movsq(reinterpret_cast<unsigned __int64 *>(ptr), reinterpret_cast<const unsigned __int64 *>(var),1)

// jump to a pointer
#define JMP_DAT(ptr)                         \
  jum_target.func_num = (long long) ptr;              \
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
    size_t fake_size = 1;                    \
    __movsq(&fake_size, (const unsigned long long *)&fsize, 1);              \
    _alloca(fake_size)

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

#define GET_SP_LOC(loc)                \
  RtlCaptureContext(&sp_loc_context);  \
  loc = sp_loc_context.Rsp

#endif