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

END