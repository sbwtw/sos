
#include "port.h"

template <>
void Port<uint8_t>::write(uint8_t data)
{
    asm volatile("outb %0, %1" : : "a"(data), "Nd"(portNumber));
}

template <>
uint8_t Port<uint8_t>::read()
{
    uint8_t result;

    asm volatile("inb %1, %0" : "=a"(result) : "Nd"(portNumber));

    return result;
}

template <>
void Port<uint16_t>::write(uint16_t data)
{
    asm volatile("outw %0, %1\n"
                 "jmp 1f\n"
                 "1:"
                 "jmp 1f\n"
                 "1:" : : "a"(data), "Nd"(portNumber));
}

template <>
uint16_t Port<uint16_t>::read()
{
    uint16_t result;

    asm volatile("inw %1, %0" : "=a"(result) : "Nd"(portNumber));

    return result;
}

template <>
void Port<uint32_t>::write(uint32_t data)
{
    asm volatile("outl %0, %1" : : "a"(data), "Nd"(portNumber));
}

template <>
uint32_t Port<uint32_t>::read()
{
    uint32_t result;

    asm volatile("inl %1, %0" : "=a"(result) : "Nd"(portNumber));

    return result;
}