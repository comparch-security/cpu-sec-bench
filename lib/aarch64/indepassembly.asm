.section .text
.globl assembly_helper
.globl exit
.type exit, %function

assembly_helper:
  sub sp, sp, #0x10
  stp x29, x30, [sp]
  str x0,[sp,#0x8]
  ldp x29,x30,[sp]
  add sp,sp,#0x10
  ret