
.set IRQ_BASE, 0x20

.section .text

.extern handleInterrupt
.global ignoreInterruptRequest

.macro HandleException number
.global _ZN16InterruptManager16handleException\number\()Ev
_ZN16InterruptManager16handleException\number\()Ev:
    movb $\number, (interrupt_number)
    jmp int_bottom
.endm

.macro HandleInterruptRequest number
.global handleInterruptRequest\number\()
handleInterruptRequest\number\():
    movb $\number + IRQ_BASE, (interrupt_number)

    pushl $0

    jmp int_bottom
.endm

HandleInterruptRequest 0x00
HandleInterruptRequest 0x01 # keyboard
HandleInterruptRequest 0x0c # mouse

int_bottom:
    #pusha
    #pushl %ds
    #pushl %es
    #pushl %fs
    #pushl %gs

    pushl %ebp
    pushl %edi
    pushl %esi

    pushl %edx
    pushl %ecx
    pushl %ebx
    pushl %eax

    pushl %esp
    push (interrupt_number)
    call handleInterrupt
    # addl $5, %esp
    movl %eax, %esp # switch stack

    popl %eax
    popl %ebx
    popl %ecx
    popl %edx

    popl %esi
    popl %edi
    popl %ebp

    #popl %gs
    #popl %fs
    #popl %es
    #popl %ds
    #popa

    add $4, %esp

ignoreInterruptRequest:

    iret

.data
    interrupt_number: .byte 0
