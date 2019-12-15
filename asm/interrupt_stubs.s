
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
    # 此处会有隐式的中断数据，由 cpu 压栈，在 multitasking.h 中定义
    # 构造 CpuRegisters 结构，在 multitasking.h 中定义，和中断数据一起处理 
    pushl %ebp   
    pushl %edi
    pushl %esi
    
    pushl %edx
    pushl %ecx
    pushl %ebx
    pushl %eax

    # 保存 ds
    mov %ds, %ax
    pushl %eax
    # 构造完成

    # handleInterrupt 的参数
    pushl %esp                  # CpuRegisters 结构
    push (interrupt_number)     # 中断号
    call handleInterrupt

    movl %eax, %esp # 处理来自中断的切换请求

    # 对应上面的寄存器恢复
    popl %eax

    popl %eax
    popl %ebx
    popl %ecx
    popl %edx

    popl %esi
    popl %edi
    popl %ebp
    # 恢复完成

    # 平衡栈
    add $4, %esp

ignoreInterruptRequest:

    iret

.data
    interrupt_number: .byte 0
