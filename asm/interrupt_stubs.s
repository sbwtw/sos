
.set IRQ_BASE, 0x20

.section .text

.extern _ZN16InterruptManager15handleInterruptEhj
.global _ZN16InterruptManager22ignoreInterruptRequestEv

.macro HandleException number
.global _ZN16InterruptManager16handleException\number\()Ev
_ZN16InterruptManager16handleException\number\()Ev:
    movb $\number, (interrupt_number)
    jmp int_bottom
.endm

.macro HandleInterruptRequest number
.global _ZN16InterruptManager26handleInterruptRequest\number\()Ev
_ZN16InterruptManager26handleInterruptRequest\number\()Ev:
    movb $\number + IRQ_BASE, (interrupt_number)
    jmp int_bottom
.endm

HandleInterruptRequest 0x00
HandleInterruptRequest 0x01 # keyboard
HandleInterruptRequest 0x0c # mouse

int_bottom:
    pusha
    pushl %ds
    pushl %es
    pushl %fs
    pushl %gs

    pushl %esp
    push (interrupt_number)
    call _ZN16InterruptManager15handleInterruptEhj
    # addl $5, %esp
    movl %eax, %esp

    popl %gs
    popl %fs
    popl %es
    popl %ds
    popa

_ZN16InterruptManager22ignoreInterruptRequestEv:

    iret

.data
    interrupt_number: .byte 0
