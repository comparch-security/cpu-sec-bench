.section .text
.globl assembly_return_site
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