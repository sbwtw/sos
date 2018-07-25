
.section .rodata
    .align 8
hexadecimal_table:
    .string "0123456789ABCDEF"

.section .text
    .extern putc
    .global printf

printf:
    push %ebp
    mov %esp, %ebp

    sub $8, %esp
    movl $8, -4(%ebp) # used for count writed length

    push %esi
    push %edi

    xor %eax, %eax
    xor %ecx, %ecx

    mov -4(%ebp), %edx
    mov (%ebp, %edx), %esi

process:
    movb (%esi, %ecx), %dl

    test %dl, %dl
    jz end

    # test format character, 0x25 == '%'
    cmpb $0x25, %dl
    je format

    push %ecx
    push %eax
    push %edx
    call putc
    #add $4, %esp
    pop %eax
    pop %ecx

    inc %ecx
    inc %eax
    jmp process

format:
    inc %ecx # eat '%' character
    movb (%esi, %ecx), %dl

    push %eax

    # take arg
    addl $4, -4(%ebp)
    mov -4(%ebp), %eax
    mov (%ebp, %eax), %eax

    # digit, 0x64 == 'd'
    cmpb $0x64, %dl
    je format_digit

    # char, 0x63 == 'c'
    cmpb $0x63, %dl
    je format_char

    # string, 0x73 == 's'
    cmpb $0x73, %dl
    je format_string

    # hex, 0x78 == 'x'
    cmpb $0x78, %dl
    je format_hexadecimal

format_end:
    pop %eax
    jmp process

format_hexadecimal:
    inc %ecx # eat 'x' character
    push %ecx # save process offset
    push %esi

    push %eax # save data
    push $0x78 # 'x'
    push $0x30 # '0'
    call putc
    call putc # output "0x"

    pop %eax # restore data
    test %eax, %eax
    jz format_hex_zero

    movl $0, -8(%ebp) # record character length

format_hex_next:
    test %eax, %eax
    jz format_hex_output

    mov $16, %esi
    cdq # sign-extend for division
    div %esi
    push %edx

    addl $1, -8(%ebp)
    jmp format_hex_next

format_hex_zero:
    push $0
    movl $1, -8(%ebp)

format_hex_output:
    mov -8(%ebp), %esi
    test %esi, %esi
    jz format_hex_end

format_hex_output_next:
    pop %eax
    add $hexadecimal_table, %eax
    mov (%eax), %eax
    push %eax
    call putc

    dec %esi
    jnz format_hex_output_next

format_hex_end:
    pop %esi
    pop %ecx # restore process offset
    jmp format_end

format_digit:
    inc %ecx

    # 0x30 == '0'
    add $0x30, %eax

    push %ecx
    push %eax
    call putc
    pop %ecx

    jmp format_end

format_char:
    inc %ecx

    push %ecx
    push %eax
    call putc
    pop %ecx

    jmp format_end

format_string:
    inc %ecx
    push %ecx

format_string_next:
    movb (%eax), %cl

    test %cl, %cl
    je format_string_end

    push %eax
    push %ecx
    call putc
    pop %eax
    inc %eax
    jmp format_string_next

format_string_end:
    pop %ecx
    jmp format_end

end:
    pop %edi
    pop %esi

    mov %ebp, %esp
    pop %ebp
    ret
