// assembly helper functions
// x86_64

// declare a label in assembly
#define DECL_LABEL(label) asm volatile(#label ":")

// jump to a label
#define JMP_LABEL(label) asm volatile("jmp " #label ";")
