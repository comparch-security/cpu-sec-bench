.section .text
.globl assembly_helper
.globl exit
.type exit, %function

assembly_helper:
  # Save address
  movq %rdi, %rax
  # Pop the return address from the stack
  popq %rbx
  # Save the fake return address to the stack
  pushq %rax
  ret