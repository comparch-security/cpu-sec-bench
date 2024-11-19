.code _text

option casemap:none

; push_fake_ret(fake_label, fsize)
push_fake_ret PROC PUBLIC
; load return address in rax
                pop     rax;
                mov     r8,1;
loop_start:     cmp     rdx,r8;
                jl      exitcode;
                push    rcx;
                add     r8,1;
                jmp     loop_start;

exitcode:       push rax;
                ret

push_fake_ret ENDP

assembly_helper PROC PUBLIC
                ; Save address
                mov     rax,rdi;
                ; Pop the return address from the stack
                pop     rbx;
                ; Save the fake return address to the stack
                push    rax;
                ret;
assembly_helper ENDP

assembly_return_site PROC PUBLIC
                ; Save the stack info
                push    rbp;
                mov     rbp,rsp;
                ; Set exit code
                xor     rdi,rdi;
                call exit
                ; Restore stack info
                mov     rsp,rbp;
                pop     rbp;
                ret;
assembly_return_site ENDP

END