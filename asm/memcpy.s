/* vim: set ft=gas: */

.section .rodata
    .align 8

.section .text
    .global memcpy

memcpy:
    pushl    %ebp
    movl    %esp, %ebp

    movl    8(%ebp), %eax # argument 1, dst
    movl    16(%ebp), %ecx # argument 3, size
    cmp     $0, %ecx
    je      end

    push    %ebx
    push    %esi
    push    %edi

    movl    12(%ebp), %esi # argument 2, src
    movl    %eax, %edi

# copy
s:
    movb    (%esi), %bl
    movb    %bl, (%edi)
    inc     %esi
    inc     %edi
    loop    s

    pop     %edi
    pop     %esi
    pop     %ebx

end:
    pop     %ebp
    ret     $12
