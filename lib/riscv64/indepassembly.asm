.section .text
.globl assembly_helper
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