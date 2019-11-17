
#include "comport.h"

ComPort::ComPort(uint16_t com)
    : _comPort(com) {
}

void ComPort::init() {
    outb(_comPort + 1, 0x00);    // Disable all interrupts
    outb(_comPort + 3, 0x80);    // Enable DLAB (set baud rate divisor)
    outb(_comPort + 0, 0x03);    // Set divisor to 3 (lo byte) 38400 baud
    outb(_comPort + 1, 0x00);    //                  (hi byte)
    outb(_comPort + 3, 0x03);    // 8 bits, no parity, one stop bit
    outb(_comPort + 2, 0xC7);    // Enable FIFO, clear them, with 14-byte threshold
    outb(_comPort + 4, 0x0B);    // IRQs enabled, RTS/DSR set
}

void ComPort::write(char c) {
    while (isTransmitEmpty() == 0)
        asm ("nop");

    outb(_comPort, c);
}

int ComPort::isTransmitEmpty() {
    return inb(_comPort + 5) & 0x20;
}

void ComPort::write(char *s) {
    while (*s != '\0')
        write(*s++);
}
