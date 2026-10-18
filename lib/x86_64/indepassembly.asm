.section .text
.globl assembly_helper
.globl assembly_return_site
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
.size assembly_helper, .-assembly_helper

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
.size assembly_return_site, .-assembly_return_site
