
#include "port.h"

inline void delay(uint32_t n)
{
    asm volatile("movl %0, %%ecx\n"
                 "start_delay:\n"
                 "nop\n"
                 "jmp delay1\n"
                 "delay1:\n"
                 "nop\n"
                 "jmp delay2\n"
                 "delay2:\n"
                 "loop start_delay" : : "c"(n));
}

template <>
void Port<uint8_t>::write(uint8_t data)
{
    asm volatile("outb %0, %1" : : "a"(data), "Nd"(_portNumber));
}

template <>
uint8_t Port<uint8_t>::read()
{
    uint8_t result;

    asm volatile("inb %1, %0" : "=a"(result) : "Nd"(_portNumber));

    return result;
}

template <>
void Port<uint16_t>::write(uint16_t data)
{
    asm volatile("outw %0, %1" : : "a"(data), "Nd"(_portNumber));
}

template <>
uint16_t Port<uint16_t>::read()
{
    uint16_t result;

    asm volatile("inw %1, %0" : "=a"(result) : "Nd"(_portNumber));

    return result;
}

template <>
void Port<uint32_t>::write(uint32_t data)
{
    asm volatile("outl %0, %1" : : "a"(data), "Nd"(_portNumber));
}

template <>
uint32_t Port<uint32_t>::read()
{
    uint32_t result;

    asm volatile("inl %1, %0" : "=a"(result) : "Nd"(_portNumber));

    return result;
}

template <>
void PortSlow<uint8_t>::write(uint8_t data) {
    asm volatile("outb %0, %1" : : "a"(data), "Nd"(_portNumber));

    delay(100);
}

template <>
void PortSlow<uint16_t>::write(uint16_t data) {
    asm volatile("outw %0, %1" : : "a"(data), "Nd"(_portNumber));

    delay(100);
}
