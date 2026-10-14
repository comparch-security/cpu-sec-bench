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
  sub sp, sp, #0x10
  stp x29, x30, [sp]
  # Set exit code
  mov x0, #0
  bl exit
  # Restore stack info
  ldp x29, x30, [sp]
  add sp, sp, #0x10
  ret
