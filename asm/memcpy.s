
.section .rodata
    .align 8

.section .text
    .global memcpy

memcpy:
    push    %ebp
    movl    %esp, %ebp

    movl    8(%ebp), %eax
    movl    16(%ebp), %ecx
    cmp     $0, %ecx
    je      ret

    push    %ebx
    push    %esi
    push    %edi

    movl    12(%ebp), %esi
    movl    %eax, %edi

s:
    movl    (%esi), %ebx
    movl    %ebx, (%edi)
    inc     %esi
    inc     %edi
    loop    s

    pop     %edi
    pop     %esi
    pop     %ebx

ret:
    pop     %ebp
    ret     $12

