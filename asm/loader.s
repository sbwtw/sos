; https://wiki.osdev.org/Multiboot

.set MAGIC, 0x1badb002
.set FLAGS, (1<<0 | 1<<1) # align 4K, save mem_* info
.set CHECKSUM, -(MAGIC + FLAGS)

.section .multiboot
    .long MAGIC
    .long FLAGS
    .long CHECKSUM

.section .text

# 内核 C 入口
.extern kernelMain
# 内核静态变量初始化
.extern callConstructors
# 入口
.global bootloader

bootloader:
    cli # 关中断

    mov $kernel_stack, %esp # 设置内核栈地址
    mov $0, %ebp; # 清空桢地址
    and $0xfffffff0, %esp # 栈地址对齐到 16 字节

    call callConstructors # 初始化内核全局变量

    push %eax # magic number
    push %ebx # multiboot 指针
    call kernelMain # 调用内核入口

_stop:
    cli
    hlt
    jmp _stop

.section .bss
.space 2 * 1024 * 1024 # 2 MiB

kernel_stack:
