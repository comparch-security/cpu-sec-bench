.section .text
.globl assembly_helper
.globl assembly_return_site
.globl exit
.type exit, %function

assembly_helper:
  sub sp, sp, #0x10
  stp x29, x30, [sp]
  str x0,[sp,#0x8]
  ldp x29,x30,[sp]
  add sp,sp,#0x10
  ret

assembly_return_site:
  # Save the stack info
  push %rbp
  movq %rsp, %rbp
  # Set exit code
  xor %rdi, %rdi
  call exit
  # Restore stack info
  movq %rbp, %rsp
  pop %rbp
  ret