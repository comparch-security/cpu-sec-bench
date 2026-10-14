.section .text
.globl assembly_helper
.globl assembly_return_site
.globl exit
.type exit, %function

assembly_helper:
  # Save the stack info
  stp c29, c30, [csp, #-32]!
  # Modify the return address: overwrite the saved link-register slot
  # with the target capability
  str c0, [csp, #16]
  # Restore the stack info
  ldp c29, c30, [csp], #32
  ret

assembly_return_site:
  # Save the stack info
  stp c29, c30, [csp, #-32]!
  # Set exit code
  mov x0, #0
  bl exit
  # Restore stack info
  ldp c29, c30, [csp], #32
  ret
