
#include "port.h"

Port::Port(uint16_t port_number)
{
    portNumber = port_number;
}

Port::~Port()
{
}

Port8Bit::Port8Bit(uint16_t port_number)
    : Port(port_number)
{
}

Port8Bit::~Port8Bit()
{
}

void Port8Bit::write(uint8_t data)
{
    asm volatile("outb %0, %1" : : "a"(data), "Nd"(portNumber));
}

uint8_t Port8Bit::read()
{
    uint8_t result;

    asm volatile("inb %1, %0" : "=a"(result) : "Nd"(portNumber));

    return result;
}

Port8BitSlow::Port8BitSlow(uint16_t port_number)
    : Port8Bit(port_number)
{
}

Port8BitSlow::~Port8BitSlow()
{
}

void Port8BitSlow::write(uint8_t data)
{
    asm volatile("outb %0, %1" : : "a"(data), "Nd"(portNumber));
}

Port16Bit::Port16Bit(uint32_t port_number)
    : Port(port_number)
{
}

Port16Bit::~Port16Bit()
{
}

void Port16Bit::write(uint16_t data)
{
    asm volatile("outw %0, %1\n"
                 "jmp 1f\n"
                 "1:"
                 "jmp 1f\n"
                 "1:" : : "a"(data), "Nd"(portNumber));
}

uint16_t Port16Bit::read()
{
    uint16_t result;

    asm volatile("inw %1, %0" : "=a"(result) : "Nd"(portNumber));

    return result;
}

Port32Bit::Port32Bit(uint64_t port_number)
    : Port(port_number)
{
}

Port32Bit::~Port32Bit()
{
}

void Port32Bit::write(uint32_t data)
{
    asm volatile("outl %0, %1" : : "a"(data), "Nd"(portNumber));
}

uint32_t Port32Bit::read()
{
    uint32_t result;

    asm volatile("inl %1, %0" : "=a"(result) : "Nd"(portNumber));

    return result;
}