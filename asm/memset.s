/* vim: set ft=gas: */

.section .rodata
    .align 8

.section .text
    .global memset

memset:
    pushl    %ebp
    movl    %esp, %ebp

    movl    8(%ebp), %eax # argument 1, dst
    movl    16(%ebp), %ecx # argument 3, n
    cmp     $0, %ecx
    je      end

    movl    12(%ebp), %edx # argument 2, src

# copy
s:
    movb    %dl, (%eax)
    inc     %eax
    loop    s

end:
    pop     %ebp
    ret     $12
