
.section .text
.extern putc
.global __sos_builtin_printf

__sos_builtin_printf:
    push %ebp
    mov %esp, %ebp

    mov $0, %eax
    mov $0, %ecx

    mov 8(%ebp), %esi

process:
    movb (%esi, %ecx), %al

    test %al, %al
    jz end

    inc %ecx

    push %ecx
    push %eax
    call putc
    #add $4, %esp
    pop %ecx

    jmp process

end:
    mov %ecx, %eax

    mov %ebp, %esp
    pop %ebp
    ret
