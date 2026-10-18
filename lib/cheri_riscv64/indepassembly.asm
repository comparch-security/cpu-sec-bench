.section .text
.globl assembly_helper
.globl assembly_return_site
.globl exit
.type exit, @function

assembly_helper:
  addi sp, sp, -16
  # save return address
  sd ra, 8(sp)
  # save frame pointer
  sd s0, 0(sp)
  # modify return address
  sd a0, 8(sp)
  # restore frame pointer
  ld s0, 0(sp)
  # restore return address
  ld ra, 8(sp)
  addi sp, sp, 16
  ret
.size assembly_helper, .-assembly_helper

assembly_return_site:
  # Save the stack info
  addi sp, sp, -16
  sd ra, 8(sp)
  sd s0, 0(sp)
  # Set exit code
  li a0, 0
  call exit
  # Restore stack info
  ld s0, 0(sp)
  ld ra, 8(sp)
  addi sp, sp, 16
  ret
.size assembly_return_site, .-assembly_return_site
